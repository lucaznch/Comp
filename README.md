# UDF compiler

At the beginning of development, the repository contains source code for the Simple language in which all references to Simple have been replaced by UDF.

The steps to develop a full UDF compiler imply the adaptation of:
* the scanner (`udf_scanner.l`)
* the parser (`udf_parser.y`)
* the symbol (`targets/symbol.h`)
* the type checker (`targets/type_checker.cpp`)
* the XML writer (for the middle delivery: `targets/xml_writer.cpp`)
* the Postfix writer (for the final delivery: `targets/postfix_writer.cpp`)
