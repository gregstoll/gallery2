FROM php:fpm

RUN docker-php-ext-install mysqli gettext

RUN pecl install xdebug && docker-php-ext-enable xdebug
