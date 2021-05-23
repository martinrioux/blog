---
layout: post
title: Docker Backups with CRON
category: Tips & Tricks
tags: [Docker, Tips & Tricks, cron]
---
Keeping permanent data in docker can sometime be challenging.
The first step is to identify all folders needed to be permanent. More often than not, these informations are available from the provider of the docker image.

| ![docker](/public/img/generic/docker.png) | ![cron](/public/img/posts/cron.jpg) |
{: .no-border-no-resize}

<!-- more -->

## Setup
Taking a Postgres docker as an example, here is a basic docker-compose config
```yml
version: "3.8"

services:
  postgres_server:
    image: postgres:12
    volumes:
      - db-data:/var/lib/postgresql/data/pgdata
    environment:
      - PGDATA=/var/lib/postgresql/data/pgdata
      - POSTGRES_PORT=5432
      - POSTGRES_USER=postgres
      - POSTGRES_PASSWORD=your_password
      - POSTGRES_DB=postgres
    ports:
      - 5432:5432
    restart: unless-stopped

volumes:
  db-data:
```

Notice that the volume `db-data` is created as a permanent volume.
If you execute `docker-compose down`, even with the `-v` argument (which will destroy all unnamed volumes), your data will be kept intact and still available on your next docker-compose up. Also, this will prevent your data from getting lost on upgrading the docker to a new version, since docker usually generate new unnamed volumes if there is any change in the configuration.

The actual volume name is actually formated as `<project_name>_<volume_name>` where the project_name is by default the basename of the folder where the docker-compose file is located. E.g.: /my_app/docker-compose would generate the volume `my_app_db-data`. You can edit the project_name by setting the environment variable `COMPOSE_PROJECT_NAME=<desired_name>`, but I suggest keeping it simple and using the folder name.

## Backuping
Even with a named volume created, the location of the named volumes and the file type make it difficult to easily backup and restore its content. You could be tempted to mount a volume to a folder, such as
```yml
volumes:
      - /home/my_user/my_project/pgdata:/var/lib/postgresql/data/pgdata
```
But the permission of the files in the folder are going to be set for the user inside the Docker, creating a folder hard to work with.
In some case mounting to a local folder works flawlessly, but first check the info provided for the image.

So, where do we go from there? In fact, the recommended way to backup the data inside a volume according to Docker is to spawn a new Docker instance, mount the desired volume, mount a local folder, and compress everything inside the mounted volume as a .tar file.

And here is an easy to implement and generic script that I use for this purpose.

### backup.sh
```bash
#!/bin/bash
#
# Docker Volume Backup
# By Martin Rioux, 2021
# Generate a dated .tar backup of docker volumes.
# Need to be put in the same folder as the docker-compose.

# ------------ CONFIG ------------
# Volume names to backup
VOLUMES="vol1 vol2 vol3"
# Backup destination folder
BACKUP_STORAGE="/backup/folder/path"
# Automatic deletion of older backups, 0 to keep everything.
NUMBER_OF_FILE_TO_KEEP=0
# NOTE: Edit PROJECT_NAME if COMPOSE_PROJECT_NAME specified
# --------------------------------

# Path to the project (automatic detection from the script location)
WORKING_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
# Project Name (folder name of the project if COMPOSE_PROJECT_NAME unspecified )
PROJECT_NAME=$( basename $WORKING_DIR )

cd $WORKING_DIR
FILEDATE=$(date +'%Y-%m-%d_%H-%M-%S')
for VOLUME in $VOLUMES;do
    docker run --rm \
    --volume ${PROJECT_NAME}_${VOLUME}:/to_backup \
    --volume ${BACKUP_STORAGE}:/generated_backup/ \
    ubuntu tar cvf /generated_backup/${PROJECT_NAME}_${VOLUME}_${FILEDATE}.tar /to_backup
    if [[ $NUMBER_OF_FILE_TO_KEEP -gt 0 ]]; then
      ( cd $BACKUP_STORAGE && ls -tp ${BACKUP_STORAGE} | grep "${PROJECT_NAME}_${VOLUME}_" | grep -v '/$' | tail -n +$(( ${NUMBER_OF_FILE_TO_KEEP} + 1 )) | xargs -d '\n' -r rm -- )
    fi
done

```

### restore.sh
```bash
#!/bin/bash
#
# Docker Volume Restore
# By Martin Rioux, 2021
# Restore a .tar backup to a volume
# Need to be put in the same folder as the docker-compose.

# ------------ CONFIG ------------
# NOTE: Edit PROJECT_NAME if COMPOSE_PROJECT_NAME specified
# --------------------------------


# Path to the project (automatic detection from the script location)
WORKING_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
# Project Name (folder name of the project if COMPOSE_PROJECT_NAME unspecified )
PROJECT_NAME=$( basename $WORKING_DIR )


USAGE="ERROR
Usage:
restore.sh <volume_name> <file>
"

if [[ -z $1 || -z $2 ]];then
    echo "$USAGE";
    exit 1
fi

docker run --rm --volume ${PROJECT_NAME}_${1}:/restore_to --volume $( realpath ${2}):/bkp_folder/$( basename ${2}) ubuntu tar xvf /bkp_folder/$(basename ${2}) -C /restore_to --strip 1

```

Create a backup.sh and a restore.sh in all docker-compose project where you need it.
Make sure they are executable with ```chmod +x backup.sh restore.sh```.

## Where does CRON fit?
We now have handy scripts to backup and restore a volume, but manually executing the backup process is not really an option. Here is where CRON fits in.

Usually include in most (if not all) major Linux distributions, cron allows the user to execute a script according [to a schedule](https://en.wikipedia.org/wiki/Cron).

In our case, lets say we want to backup the volumes et 2 am every day.
To enter a new cron job, execute `cron -e`.

Append this to the file:
(Do not forget to change the path to where your backup.sh script is located.)
```bash
SHELL=/bin/bash
0 2 * * * /path/to/project/backup.sh
```
Save the changes and thats it! Your script will be executed daily and your volumes will be backed up!


## How to restore?
To restore, cd into your docker-compose project and execute
```bash
./restore.sh <volume_to_restore> <restore_file>
```

Do not forget to set the PROJECT_NAME in the restore.sh if you specify the COMPOSE_PROJECT_NAME.

When you want to restore to a new instance, start the docker-compose (up), bring it back down, restore the volume and start everyting back again.

## Dockerizing the backup service
You could actually add a new service in the docker-compose file to generate a Docker which would include cron and the backup job. I didn't go that way because I found that this needed more configuration, more files and would make the restore process harder to understand. Also, the availability of cron makes the setup a breeze.
