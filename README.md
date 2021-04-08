EU Tire Label
=============

According to the European Tire Labeling Regulation
([EC/1222/2009](http://eur-lex.europa.eu/LexUriServ/LexUriServ.do?uri=OJ:L:2009:342:0046:0058:en:PDF))
all manufacturers of tires for cars, light and heavy trucks are obliged to specify fuel
efficiency, wet grip and external rolling noise classification of every tire sold in European
Union market after October 2012. Those information should be presented to the customer in the form
of Tire Label designed by the EU Commission.

This project (EU-tire-label) aims to simplify label generation and distribution in the web-based
services (manufacturer website, E-commerce, etc.). For further information see the 'Usage' section.

Installation
------------

```sh
mkdir build && cd build
cmake -DENABLE_CGI=ON -DENABLE_PNG=ON ..
make && make install
```

Libraries required for PNG output:

* [cairo](http://cairographics.org/) compiled with SVG and PNG support
* [librsvg](https://wiki.gnome.org/Projects/LibRsvg)

Usage
-----

As a standalone executable:

```sh
eu-tire-label --tire-class=1 --fuel-efficiency=B --wet-grip=E \
    --rolling-noise=2 --rolling-noise-db=72 >tire-label-1-B-E-2-72.svg
eu-tire-label --tire-class=1 --fuel-efficiency=B --wet-grip=E \
    --rolling-noise=2 --rolling-noise-db=72 --output-png=350 >tire-label-1-B-E-2-72.png
```

As a CGI application using e.g. Apache HTTP server. Note, that for convenience the query string is
case insensitive.

```sh
wget "http://localhost/cgi-bin/eu-tire-label?c=1&f=b&g=e&r=2&n=72"
wget "http://localhost/cgi-bin/eu-tire-label?c=1&f=b&g=e&r=2&n=72&png=350"
```
