---
layout: post
title: Configurating Grafana on Docker
category: Tips & Tricks
tags: [Grafana, Tips & Tricks, Docker]
---
The Docker version of Grafana doesn't have a lot of information about the custom configurations usually found in the `grafana.ini` config file. Editing or replacing the `/etc/grafana/grafana.ini` file inside the docker is finicky and not the proper way since Grafana specifies it should be made through the environment variables. Alas, I wasn't able to find a list anywhere of all the options.
Turns out there is a logic to it!

| ![docker](/public/img/generic/docker.png) | ![grafana](/public/img/generic/grafana.png) |
{: .no-border .small}

<!-- more -->

I'll start by showing my docker-compose and configuration. Note that it include a renderer server for the alerts.

Here are the revelant par of the `docker-compose.yml`
```yml
version: "3.8"

volumes:
  grafana-data:

services:
  grafana:
    image: 'grafana/grafana:7.4.3'
    ports:
      - "${GRAFANA_PORT-3000}:3000"
    env_file:
      - .env
    volumes:
      - grafana-data:/var/lib/grafana
    restart: unless-stopped

  renderer:
      image: "grafana/grafana-image-renderer:2.0.1"
      ports:
        - "${GRAFANA_RENDERER_PORT-8081}"

```

and for my .env file
```ini
# GRAFANA
GRAFANA_PORT=3000
GF_PANELS_DISABLE_SANITIZE_HTML=false
# Grafana Alert SMTP Config (set to true and configure)
GF_SMTP_ENABLED=false
GF_SMTP_HOST=smtp.gmail.com:465
GF_SMTP_USER=your@user.com
GF_SMTP_PASSWORD=your_password
# Rendering server for alerts and embedding
GF_SECURITY_ALLOW_EMBEDDING=false
GRAFANA_RENDERER_PORT=8081
GF_RENDERING_SERVER_URL=http://renderer:${GRAFANA_RENDERER_PORT}/render
GF_RENDERING_CALLBACK_URL=http://grafana:${GRAFANA_PORT}/
# Allow anonymous login
GF_AUTH_ANONYMOUS_ENABLED=false
GF_AUTH_ANONYMOUS_ORG_NAME=your_org_name
GF_AUTH_ANONYMOUS_ORG_ROLE=viewer
```

More specifically, I was looking into getting the anonymous user to a specific organization.

By deduction and trial and error, I found that the env var was `GF_AUTH_ANONYMOUS_ORG_ROLE=<org_name>`

Looking in grafana at the URL `http://<hostname>/admin/settings`, I saw:

![docker](/public/img/posts/grafana_config1.jpg)

So I went ahead and tried set the azuread allow_sign_up env var with the same logic by adding `GF_AUTH_AZUREAD_ALLOW_SIGN_UP=false`

![docker](/public/img/posts/grafana_config2.jpg)

With the same logic, we most likely can set any option we want!
