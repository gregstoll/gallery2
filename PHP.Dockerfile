FROM php:8.0-fpm

RUN docker-php-ext-install mysqli gettext

RUN pecl install xdebug && docker-php-ext-enable xdebug

RUN apt-get update && apt-get install -y \
		libfreetype6-dev \
		libjpeg62-turbo-dev \
		libpng-dev \
		vim \
    && docker-php-ext-configure gd --with-freetype --with-jpeg \
	&& docker-php-ext-install -j$(nproc) gd

RUN apt-get install -y libjpeg-progs imagemagick-6
