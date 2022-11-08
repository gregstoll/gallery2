# Branches

* `dev/*` branches: branches that are intended for new code to be integrated on the upstream repository via PR.

# Development environment

Check it at the [Archive of the
Internet](`https://web.archive.org/web/20210224144709/http://codex.gallery2.org/index.php/Gallery2:Developer_Guidelines`)

# Unit testing

Run unit testing by navigating to the following URL:
```text
http://SITE_ADDRESS/lib/tools/phpunit/index.php
```

To run any test, write the name of the module on the input box and hit `ENTER` (there is no form button).

## Docker

There is a docker compose configuration that will run nginx web server, PHP-FPM server and MySQL server.

In order to start it run:

```shell
docker compose up -d
```

## Initial setup

Point your browser to `http://host.docker.internal:10000/install.php` and follow the instructions.

Use `/gallery2-data` on the Storage setup step. Make sure the directory is writable, since when initially
created it will not allow writing.

```shell
$ docker exec -it gallery2-php8-php-1 bash -c "chmod a+w /gallery2-data"
```

You will find mysql credentials in the `docker-compose.yaml` file. Use `mysql` for the server hostname.


## Configuration

Add the following configuration to `config.php` file:

```php
$gallery->setConfig('baseUri', 'http://host.docker.internal:10000/main.php');
```

This way you will be able to access the web server running on docker, and at the same time, you will be
able to debug the code.

## Locale files

Some unit tests require translation files for the core module.

In order to facilitate the initial setup, there is a script at `lib/tools/phpunit/create-locale.sh` to
create the files in the appropriate location.

# Release management

Tools for release management are on folder `lib/tools/release`.

# Translation tools

Tools for release management are on folder `lib/tools/po`.

See further details at [Gallery2 Codex](http://codex.galleryproject.org/Gallery2:Localization.html).
