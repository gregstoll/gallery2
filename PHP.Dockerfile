FROM php:8.0-fpm

RUN docker-php-ext-install mysqli gettext

RUN pecl install xdebug && docker-php-ext-enable xdebug
