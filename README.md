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

	$ autoreconf --install
	$ mkdir build && cd build
	$ ../configure --enable-cgi
	$ make && make install

Usage
-----
