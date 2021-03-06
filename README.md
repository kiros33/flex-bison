# flex & bison

[Book on O'Reilly](http://shop.oreilly.com/product/9780596155988.do)  
![flex & bison](https://user-images.githubusercontent.com/1563133/52050641-8a77b800-2594-11e9-89f3-6499281be157.gif)  
[flexbison.zip](https://github.com/kiros33/flex-bison/files/2817025/flexbison.zip)  

### Cross-complie Test
| Project | macOS | Linux | Windows | Descriptions |
| :--- | :---: | :---: | :---: | :--- |
| fb1-1 | O | | ? | |
| fb1-2 | O | | | |
| fb1-3 | O | | | |
| fb1-4 | O | | ? | |
| fb1-5 | O | | ? | |
| fb2-1 | O | | | |
| fb2-1-cmake | O | | O | * use cmake |
| fb2-2 | O | | | |
| fb2-3 | O | | O | |
| fb2-4 | O | | | |
| fb2-5 | O | | | |
| fb3-1 | O | | | |
| fb3-2 | O | | O | * use cmake<br>* add arguments processing with getopt<br>* accept block and inline comments |
| fb4-1 | O | | O | |
| fb9-1 | O | | O | fb2-1 기반 Pure Scanner 구현 |
| fb9-2 | O | | O | fb3-2 기반 Pure Scanner & Pure Parser 구현 |

commit at windows

### Branch

| Branch Name | Descriptions |
| :--- | :--- |
| typedef | redefine all sturct by typedef and apply all source code |
| fb3-2-default | **Source code without additional functions<br><br>The 'fb3-2' project has been developed with additional functions.<br>  - Compiling at Visual Studio<br>  - Block and Inline Comments<br>  - Arguments processing with getopt()  |

* checkout/push

```
git checkout -t origin/typedef
```

```
git push origin typedef
```

### Source code for macOS and Windows
* macOS(Terminal)
  > install flex and bison via brew
* Windows(Visual Studio 2015 Community)
  > winflex/winbison

### Versions on books

* Flex: The current version as of early 2009 was 2.5.35.
* Bison: The current version as of early 2009 was 2.4.1.

### Current project versions
* Flex

```bash
$ flex --version
flex 2.5.37
```

* Bison

```bash
$ bison --version
bison (GNU Bison) 3.0.4
Written by Robert Corbett and Richard Stallman.

Copyright (C) 2015 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

### files
Source files: [flexbison.zip](https://github.com/kiros33/flex-bison/files/2817025/flexbison.zip)

flexbison.zip
```text
This file contains the source code for the programs in "flex and
bison" published by O'Reilly in 2009.  If you use this code in a
project or product I would appreciate credit, and, of course, it would
be nice if you bought a copy of the book.

The Makefiles are written for the widely available GNU Make.  The
code has been tested with flex 2.5.35 and bison 2.4.1.  If you find
bugs in this code, please send e-mail (preferably with fixes) so I can
note it in errata and fix it in a future version.

John Levine, November 2009
Taughannock Networks
PO Box 727
Trumansburg NY 14886 US
fbook@iecc.com

Book info:  http://oreilly.com/catalog/9780596155971/
This code: ftp://ftp.iecc.com/pub/file/flexbison.zip

---- 

Copyright (c) 2009, Taughannock Networks. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of Taughannock Networks nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

$Header: /home/johnl/flnb/code/RCS/README,v 2.3 2009/11/08 02:57:48 johnl Exp $
```
