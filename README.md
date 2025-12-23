[![Visual Studio Code](https://custom-icon-badges.demolab.com/badge/Visual%20Studio%20Code-0078d7.svg?logo=vsc&logoColor=white)](#)
[![Read the Docs](https://img.shields.io/badge/Read%20the%20Docs-8CA1AF?logo=readthedocs&logoColor=fff)](#)
[![macOS](https://img.shields.io/badge/macOS-000000?logo=apple&logoColor=F0F0F0)](#)
[![Linux](https://img.shields.io/badge/Linux-FCC624?logo=linux&logoColor=black)](#)

[![CMake](https://img.shields.io/badge/CMake-064F8C?logo=cmake&logoColor=fff)](#)
[![C++](https://img.shields.io/badge/-C++-blue?logo=cplusplus)](#)
[![SQLite](https://img.shields.io/badge/SQLite-%2307405e.svg?logo=sqlite&logoColor=white)](#)

[![version](https://img.shields.io/badge/version-0.2.2-blue)](#)
[![passing](https://img.shields.io/badge/build-passing-green)](#)
[![testing](https://img.shields.io/badge/coverage-N/A-yellow)](#)
[![secure](https://img.shields.io/badge/secure-very-green)](#)


<div align="center">

<img src="https://raw.githubusercontent.com/TrynaThinkOf1/phi/e8bea8aebf333dbea9d54ebae7892afb50c9369d/assets/phi_logo.svg" alt="logo" width="250">

<h1 color="#2e083b">PHI</h1>

<h5 style="color:#fbb040">Secure messaging app that lives in your terminal.</h5>

</div>


<h1>📊 FEATURES</h1>

<h5>👤 USER EXPERIENCE</h5>

* SQLite3 database to hold user data - [SQLiteC++](https://github.com/SRombauts/SQLiteCpp)
	* Password protected
* Advanced and intuitive Terminal User Interface - [FTXUI](https://arthursonzogni.github.io/FTXUI/)

<h5>🔒 SECURITY</h5>

* GZip (L3) compression for messages - [zlib](https://zlib.net)
* ChaCha20-Poly1305 encryption for messages - [libsodium](https://doc.libsodium.org/secret-key_cryptography/aead/chacha20-poly1305)
* RSA (4096) encryption for CC20 session keys - [Crypto++](https://www.cryptopp.com/wiki/RSA_Cryptography)
	* Provides performant and secure hybrid encryption, inspiration from PGP
* BLAKE2b signature to ensure no tampering - [Crypto++](https://www.cryptopp.com/wiki/BLAKE2)
	* Also uses MAC (with shared secret) signature to ensure sender integrity - [libsodium](https://doc.libsodium.org/secret-key_cryptography/secret-key_authentication)
		* Diffie-Hellmann key exchange technique for shared secrets - [libsodium](https://doc.libsodium.org/key_exchange)