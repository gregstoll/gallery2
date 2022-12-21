FROM php:8.1-fpm

RUN docker-php-ext-install mysqli gettext

RUN pecl install xdebug && docker-php-ext-enable xdebug

RUN apt-get update && apt-get install -y \
		libfreetype6-dev \
		libjpeg62-turbo-dev \
		libpng-dev \
		vim \
		imagemagick\
		libjpeg-turbo-progs \
	&& docker-php-ext-configure gd --with-freetype --with-jpeg \
	&& docker-php-ext-install -j$(nproc) gd

RUN apt-get install less

#
# Test locale support is working:
#    setlocale(LC_ALL, "fr_FR"); echo strftime("%A %d %B %Y");
#
RUN apt-get install -y libonig-dev locales && apt-get clean \
    && sed -i -e 's/# ca_ES ISO-8859-1/ca_ES ISO-8859-1/' /etc/locale.gen \
    && sed -i -e 's/# fr_FR ISO-8859-1/fr_FR ISO-8859-1/' /etc/locale.gen \
    && sed -i -e 's/# es_ES ISO-8859-1/es_ES ISO-8859-1/' /etc/locale.gen \
    && sed -i -e 's/# es_AR ISO-8859-1/es_AR ISO-8859-1/' /etc/locale.gen \
    && sed -i -e 's/# de_DE ISO-8859-1/de_DE ISO-8859-1/' /etc/locale.gen \
    && sed -i -e 's/# pt_BR ISO-8859-1/pt_BR ISO-8859-1/' /etc/locale.gen \
    && dpkg-reconfigure --frontend=noninteractive locales \
	&& docker-php-ext-install -j$(nproc) gettext mbstring

RUN docker-php-ext-install -j$(nproc) exif

# compression
RUN apt-get update \
	&& apt-get install -y \
	libbz2-dev \
	zlib1g-dev \
	libzip-dev \
	&& docker-php-ext-install -j$(nproc) \
		zip \
		bz2

# math
RUN apt-get update \
	&& apt-get install -y libgmp-dev \
	&& ln -s /usr/include/x86_64-linux-gnu/gmp.h /usr/include/gmp.h \
	&& docker-php-ext-install -j$(nproc) \
		gmp \
		bcmath

# PHP
# intl
RUN apt-get update \
	&& apt-get install -y libicu-dev \
	&& docker-php-ext-configure intl \
	&& docker-php-ext-install -j$(nproc) intl

# xml
RUN apt-get update \
	&& apt-get install -y \
	libxml2-dev \
	libxslt-dev \
	&& docker-php-ext-install -j$(nproc) \
		dom \
		xsl
