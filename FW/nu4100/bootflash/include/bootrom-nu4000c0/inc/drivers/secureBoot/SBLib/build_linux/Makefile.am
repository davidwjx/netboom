##
## File: Makefile.am
##
## Template for building Makefile with automake+configure
##

##############################################################################
## Copyright (c) 2009-2018 INSIDE Secure B.V. All Rights Reserved.
##
## This confidential and proprietary software may be used only as authorized
## by a licensing agreement from INSIDE Secure.
##
## The entire notice above must be reproduced on all authorized copies that
## may only be made to the extent permitted by a licensing agreement from
## INSIDE Secure.
##
## For more information or support, please go to our online support system at
## https://customersupport.insidesecure.com.
## In case you do not have an account for this system, please send an e-mail
## to ESSEmbeddedHW-Support@insidesecure.com.
##############################################################################

# --------------------------
# Secure Boot Configurations
# --------------------------
#                   I/P  C/P  SW/HW  AES SHA ECDSA WrapKey MaxCerts Curve SBCR
#                   ---  ---  ------ --- --- ----- ------- -------- -----------
# cfgP224IPc0s      Yes  No   SW     n/a SW  SW    n/a     0        P-224   n/a
# cfgP224IPc1s      Yes  No   SW     n/a SW  SW    n/a     1        P-224   n/a
# cfgP224IPc2s      Yes  No   SW     n/a SW  SW    n/a     2        P-224   n/a
# cfgP224CPc0s      Yes  Yes  SW     SW  SW  SW    Prg     0        P-224   128
# cfgP224CPc1s      Yes  Yes  SW     SW  SW  SW    Prg     1        P-224   128
# cfgP224CPc2s      Yes  Yes  SW     SW  SW  SW    Prg     2        P-224   128
# cfgP256IPc0s      Yes  No   SW     n/a SW  SW    n/a     0        P-256   n/a
# cfgP256IPc1s      Yes  No   SW     n/a SW  SW    n/a     1        P-256   n/a
# cfgP256IPc2s      Yes  No   SW     n/a SW  SW    n/a     2        P-256   n/a
# cfgP256CPc0s      Yes  Yes  SW     SW  SW  SW    Prg     0        P-256   128
# cfgP256CPc1s      Yes  Yes  SW     SW  SW  SW    Prg     1        P-256   128
# cfgP256CPc2s      Yes  Yes  SW     SW  SW  SW    Prg     2        P-256   128
# cfgP224IPc0sw     Yes  No   SW     n/a SW  SW    n/a     0        P-224   n/a
# cfgP224IPc1sw     Yes  No   SW     n/a SW  SW    n/a     1        P-224   n/a
# cfgP224IPc2sw     Yes  No   SW     n/a SW  SW    n/a     2        P-224   n/a
# cfgP224CPc0sw     Yes  Yes  SW     SW  SW  SW    Prg     0        P-224   128
# cfgP224CPc1sw     Yes  Yes  SW     SW  SW  SW    Prg     1        P-224   128
# cfgP224CPc2sw     Yes  Yes  SW     SW  SW  SW    Prg     2        P-224   128
# cfgP224CXPc0sw    Yes  Yes  SW     SW  SW  SW    Prg     0        P-224   256
# cfgP224CXPc1sw    Yes  Yes  SW     SW  SW  SW    Prg     1        P-224   256
# cfgP224CXPc2sw    Yes  Yes  SW     SW  SW  SW    Prg     2        P-224   256
# cfgP224CXPKc0sw   Yes  Yes  SW     SW  SW  SW    Prg     0        P-224  K256
# cfgP224CXPKc1sw   Yes  Yes  SW     SW  SW  SW    Prg     1        P-224  K256
# cfgP224CXPKc2sw   Yes  Yes  SW     SW  SW  SW    Prg     2        P-224  K256
# cfgP256IPc0sw     Yes  No   SW     n/a SW  SW    n/a     0        P-256   n/a
# cfgP256IPc1sw     Yes  No   SW     n/a SW  SW    n/a     1        P-256   n/a
# cfgP256IPc2sw     Yes  No   SW     n/a SW  SW    n/a     2        P-256   n/a
# cfgP256CPc0sw     Yes  Yes  SW     SW  SW  SW    Prg     0        P-256   128
# cfgP256CPc1sw     Yes  Yes  SW     SW  SW  SW    Prg     1        P-256   128
# cfgP256CPc2sw     Yes  Yes  SW     SW  SW  SW    Prg     2        P-256   128
# cfgP256CXPc0sw    Yes  Yes  SW     SW  SW  SW    Prg     0        P-256   256
# cfgP256CXPc1sw    Yes  Yes  SW     SW  SW  SW    Prg     1        P-256   256
# cfgP256CXPc2sw    Yes  Yes  SW     SW  SW  SW    Prg     2        P-256   256
# cfgP256CXPKc0sw   Yes  Yes  SW     SW  SW  SW    Prg     0        P-256  K256
# cfgP256CXPKc1sw   Yes  Yes  SW     SW  SW  SW    Prg     1        P-256  K256
# cfgP256CXPKc2sw   Yes  Yes  SW     SW  SW  SW    Prg     2        P-256  K256
# cfgP384IPc0sw     Yes  No   SW     n/a SW  SW    n/a     0        P-384   n/a
# cfgP384IPc1sw     Yes  No   SW     n/a SW  SW    n/a     1        P-384   n/a
# cfgP384IPc2sw     Yes  No   SW     n/a SW  SW    n/a     2        P-384   n/a
# cfgP384CPc0sw     Yes  Yes  SW     SW  SW  SW    Prg     0        P-384   128
# cfgP384CPc1sw     Yes  Yes  SW     SW  SW  SW    Prg     1        P-384   128
# cfgP384CPc2sw     Yes  Yes  SW     SW  SW  SW    Prg     2        P-384   128
# cfgP384CXPc0sw    Yes  Yes  SW     SW  SW  SW    Prg     0        P-384   256
# cfgP384CXPc1sw    Yes  Yes  SW     SW  SW  SW    Prg     1        P-384   256
# cfgP384CXPc2sw    Yes  Yes  SW     SW  SW  SW    Prg     2        P-384   256
# cfgP384CXPKc0sw   Yes  Yes  SW     SW  SW  SW    Prg     0        P-384  K256
# cfgP384CXPKc1sw   Yes  Yes  SW     SW  SW  SW    Prg     1        P-384  K256
# cfgP384CXPKc2sw   Yes  Yes  SW     SW  SW  SW    Prg     2        P-384  K256
# cfgP521IPc0sw     Yes  No   SW     n/a SW  SW    n/a     0        P-521   n/a
# cfgP521IPc1sw     Yes  No   SW     n/a SW  SW    n/a     1        P-521   n/a
# cfgP521IPc2sw     Yes  No   SW     n/a SW  SW    n/a     2        P-521   n/a
# cfgP521CPc0sw     Yes  Yes  SW     SW  SW  SW    Prg     0        P-521   128
# cfgP521CPc1sw     Yes  Yes  SW     SW  SW  SW    Prg     1        P-521   128
# cfgP521CPc2sw     Yes  Yes  SW     SW  SW  SW    Prg     2        P-521   128
# cfgP521CXPc0sw    Yes  Yes  SW     SW  SW  SW    Prg     0        P-521   256
# cfgP521CXPc1sw    Yes  Yes  SW     SW  SW  SW    Prg     1        P-521   256
# cfgP521CXPc2sw    Yes  Yes  SW     SW  SW  SW    Prg     2        P-521   256
# cfgP521CXPKc0sw   Yes  Yes  SW     SW  SW  SW    Prg     0        P-521  K256
# cfgP521CXPKc1sw   Yes  Yes  SW     SW  SW  SW    Prg     1        P-521  K256
# cfgP521CXPKc2sw   Yes  Yes  SW     SW  SW  SW    Prg     2        P-521  K256
# cfgP224IPc0sm     Yes  No   SM     n/a SM  SM    n/a     0        P-224   n/a
# cfgP224IPc1sm     Yes  No   SM     n/a SM  SM    n/a     1        P-224   n/a
# cfgP224IPc2sm     Yes  No   SM     n/a SM  SM    n/a     2        P-224   n/a
# cfgP224CPc0sm     Yes  Yes  SM     SM  SM  SM    Prg     0        P-224   128
# cfgP224CPc1sm     Yes  Yes  SM     SM  SM  SM    Prg     1        P-224   128
# cfgP224CPc2sm     Yes  Yes  SM     SM  SM  SM    Prg     2        P-224   128
# cfgP224CNc0sm     Yes  Yes  SM     SM  SM  SM    OTP     0        P-224   128
# cfgP224CNc1sm     Yes  Yes  SM     SM  SM  SM    OTP     1        P-224   128
# cfgP224CNc2sm     Yes  Yes  SM     SM  SM  SM    OTP     2        P-224   128
# cfgP224CXPc0sm    Yes  Yes  SM     SM  SM  SM    Prg     0        P-224   256
# cfgP224CXPc1sm    Yes  Yes  SM     SM  SM  SM    Prg     1        P-224   256
# cfgP224CXPc2sm    Yes  Yes  SM     SM  SM  SM    Prg     2        P-224   256
# cfgP224CXNc0sm    Yes  Yes  SM     SM  SM  SM    OTP     0        P-224   256
# cfgP224CXNc1sm    Yes  Yes  SM     SM  SM  SM    OTP     1        P-224   256
# cfgP224CXNc2sm    Yes  Yes  SM     SM  SM  SM    OTP     2        P-224   256
# cfgP224CXPKc0sm   Yes  Yes  SM     SM  SM  SM    Prg     0        P-224  K256
# cfgP224CXPKc1sm   Yes  Yes  SM     SM  SM  SM    Prg     1        P-224  K256
# cfgP224CXPKc2sm   Yes  Yes  SM     SM  SM  SM    Prg     2        P-224  K256
# cfgP224CXNKc0sm   Yes  Yes  SM     SM  SM  SM    OTP     0        P-224  K256
# cfgP224CXNKc1sm   Yes  Yes  SM     SM  SM  SM    OTP     1        P-224  K256
# cfgP224CXNKc2sm   Yes  Yes  SM     SM  SM  SM    OTP     2        P-224  K256
# cfgP256IPc0sm     Yes  No   SM     n/a SM  SM    n/a     0        P-256   n/a
# cfgP256IPc1sm     Yes  No   SM     n/a SM  SM    n/a     1        P-256   n/a
# cfgP256IPc2sm     Yes  No   SM     n/a SM  SM    n/a     2        P-256   n/a
# cfgP256CPc0sm     Yes  Yes  SM     SM  SM  SM    Prg     0        P-256   128
# cfgP256CPc1sm     Yes  Yes  SM     SM  SM  SM    Prg     1        P-256   128
# cfgP256CPc2sm     Yes  Yes  SM     SM  SM  SM    Prg     2        P-256   128
# cfgP256CNc0sm     Yes  Yes  SM     SM  SM  SM    OTP     0        P-256   128
# cfgP256CNc1sm     Yes  Yes  SM     SM  SM  SM    OTP     1        P-256   128
# cfgP256CNc2sm     Yes  Yes  SM     SM  SM  SM    OTP     2        P-256   128
# cfgP256CXPc0sm    Yes  Yes  SM     SM  SM  SM    Prg     0        P-256   256
# cfgP256CXPc1sm    Yes  Yes  SM     SM  SM  SM    Prg     1        P-256   256
# cfgP256CXPc2sm    Yes  Yes  SM     SM  SM  SM    Prg     2        P-256   256
# cfgP256CXNc0sm    Yes  Yes  SM     SM  SM  SM    OTP     0        P-256   256
# cfgP256CXNc1sm    Yes  Yes  SM     SM  SM  SM    OTP     1        P-256   256
# cfgP256CXNc2sm    Yes  Yes  SM     SM  SM  SM    OTP     2        P-256   256
# cfgP256CXPKc0sm   Yes  Yes  SM     SM  SM  SM    Prg     0        P-256  K256
# cfgP256CXPKc1sm   Yes  Yes  SM     SM  SM  SM    Prg     1        P-256  K256
# cfgP256CXPKc2sm   Yes  Yes  SM     SM  SM  SM    Prg     2        P-256  K256
# cfgP256CXNKc0sm   Yes  Yes  SM     SM  SM  SM    OTP     0        P-256  K256
# cfgP256CXNKc1sm   Yes  Yes  SM     SM  SM  SM    OTP     1        P-256  K256
# cfgP256CXNKc2sm   Yes  Yes  SM     SM  SM  SM    OTP     2        P-256  K256
# cfgP384IPc0sm     Yes  No   SM     n/a SM  SM    n/a     0        P-384   n/a
# cfgP384IPc1sm     Yes  No   SM     n/a SM  SM    n/a     1        P-384   n/a
# cfgP384IPc2sm     Yes  No   SM     n/a SM  SM    n/a     2        P-384   n/a
# cfgP384CPc0sm     Yes  Yes  SM     SM  SM  SM    Prg     0        P-384   128
# cfgP384CPc1sm     Yes  Yes  SM     SM  SM  SM    Prg     1        P-384   128
# cfgP384CPc2sm     Yes  Yes  SM     SM  SM  SM    Prg     2        P-384   128
# cfgP384CNc0sm     Yes  Yes  SM     SM  SM  SM    OTP     0        P-384   128
# cfgP384CNc1sm     Yes  Yes  SM     SM  SM  SM    OTP     1        P-384   128
# cfgP384CNc2sm     Yes  Yes  SM     SM  SM  SM    OTP     2        P-384   128
# cfgP384CXPc0sm    Yes  Yes  SM     SM  SM  SM    Prg     0        P-384   256
# cfgP384CXPc1sm    Yes  Yes  SM     SM  SM  SM    Prg     1        P-384   256
# cfgP384CXPc2sm    Yes  Yes  SM     SM  SM  SM    Prg     2        P-384   256
# cfgP384CXNc0sm    Yes  Yes  SM     SM  SM  SM    OTP     0        P-384   256
# cfgP384CXNc1sm    Yes  Yes  SM     SM  SM  SM    OTP     1        P-384   256
# cfgP384CXNc2sm    Yes  Yes  SM     SM  SM  SM    OTP     2        P-384   256
# cfgP384CXPKc0sm   Yes  Yes  SM     SM  SM  SM    Prg     0        P-384  K256
# cfgP384CXPKc1sm   Yes  Yes  SM     SM  SM  SM    Prg     1        P-384  K256
# cfgP384CXPKc2sm   Yes  Yes  SM     SM  SM  SM    Prg     2        P-384  K256
# cfgP384CXNKc0sm   Yes  Yes  SM     SM  SM  SM    OTP     0        P-384  K256
# cfgP384CXNKc1sm   Yes  Yes  SM     SM  SM  SM    OTP     1        P-384  K256
# cfgP384CXNKc2sm   Yes  Yes  SM     SM  SM  SM    OTP     2        P-384  K256
# cfgP521IPc0sm     Yes  No   SM     n/a SM  SM    n/a     0        P-521   n/a
# cfgP521IPc1sm     Yes  No   SM     n/a SM  SM    n/a     1        P-521   n/a
# cfgP521IPc2sm     Yes  No   SM     n/a SM  SM    n/a     2        P-521   n/a
# cfgP521CPc0sm     Yes  Yes  SM     SM  SM  SM    Prg     0        P-521   128
# cfgP521CPc1sm     Yes  Yes  SM     SM  SM  SM    Prg     1        P-521   128
# cfgP521CPc2sm     Yes  Yes  SM     SM  SM  SM    Prg     2        P-521   128
# cfgP521CNc0sm     Yes  Yes  SM     SM  SM  SM    OTP     0        P-521   128
# cfgP521CNc1sm     Yes  Yes  SM     SM  SM  SM    OTP     1        P-521   128
# cfgP521CNc2sm     Yes  Yes  SM     SM  SM  SM    OTP     2        P-521   128
# cfgP521CXPc0sm    Yes  Yes  SM     SM  SM  SM    Prg     0        P-521   256
# cfgP521CXPc1sm    Yes  Yes  SM     SM  SM  SM    Prg     1        P-521   256
# cfgP521CXPc2sm    Yes  Yes  SM     SM  SM  SM    Prg     2        P-521   256
# cfgP521CXNc0sm    Yes  Yes  SM     SM  SM  SM    OTP     0        P-521   256
# cfgP521CXNc1sm    Yes  Yes  SM     SM  SM  SM    OTP     1        P-521   256
# cfgP521CXNc2sm    Yes  Yes  SM     SM  SM  SM    OTP     2        P-521   256
# cfgP521CXPKc0sm   Yes  Yes  SM     SM  SM  SM    Prg     0        P-521  K256
# cfgP521CXPKc1sm   Yes  Yes  SM     SM  SM  SM    Prg     1        P-521  K256
# cfgP521CXPKc2sm   Yes  Yes  SM     SM  SM  SM    Prg     2        P-521  K256
# cfgP521CXNKc0sm   Yes  Yes  SM     SM  SM  SM    OTP     0        P-521  K256
# cfgP521CXNKc1sm   Yes  Yes  SM     SM  SM  SM    OTP     1        P-521  K256
# cfgP521CXNKc2sm   Yes  Yes  SM     SM  SM  SM    OTP     2        P-521  K256
# cfgP224IPc0pk     Yes  No   PK     n/a SW  PK    n/a     0        P-224   n/a
# cfgP224IPc1pk     Yes  No   PK     n/a SW  PK    n/a     1        P-224   n/a
# cfgP224IPc2pk     Yes  No   PK     n/a SW  PK    n/a     2        P-224   n/a
# cfgP224CPc0pk     Yes  Yes  PK     SW  SW  PK    Prg     0        P-224   128
# cfgP224CPc1pk     Yes  Yes  PK     SW  SW  PK    Prg     1        P-224   128
# cfgP224CPc2pk     Yes  Yes  PK     SW  SW  PK    Prg     2        P-224   128
# cfgP256IPc0pk     Yes  No   PK     n/a SW  PK    n/a     0        P-256   n/a
# cfgP256IPc1pk     Yes  No   PK     n/a SW  PK    n/a     1        P-256   n/a
# cfgP256IPc2pk     Yes  No   PK     n/a SW  PK    n/a     2        P-256   n/a
# cfgP256CPc0pk     Yes  Yes  PK     SW  SW  PK    Prg     0        P-256   128
# cfgP256CPc1pk     Yes  Yes  PK     SW  SW  PK    Prg     1        P-256   128
# cfgP256CPc2pk     Yes  Yes  PK     SW  SW  PK    Prg     2        P-256   128
# cfgP384IPc0pk     Yes  No   PK     n/a SW  PK    n/a     0        P-384   n/a
# cfgP384IPc1pk     Yes  No   PK     n/a SW  PK    n/a     1        P-384   n/a
# cfgP384IPc2pk     Yes  No   PK     n/a SW  PK    n/a     2        P-384   n/a
# cfgP384CPc0pk     Yes  Yes  PK     SW  SW  PK    Prg     0        P-384   128
# cfgP384CPc1pk     Yes  Yes  PK     SW  SW  PK    Prg     1        P-384   128
# cfgP384CPc2pk     Yes  Yes  PK     SW  SW  PK    Prg     2        P-384   128
# cfgP521IPc0pk     Yes  No   PK     n/a SW  PK    n/a     0        P-521   n/a
# cfgP521IPc1pk     Yes  No   PK     n/a SW  PK    n/a     1        P-521   n/a
# cfgP521IPc2pk     Yes  No   PK     n/a SW  PK    n/a     2        P-521   n/a
# cfgP521CPc0pk     Yes  Yes  PK     SW  SW  PK    Prg     0        P-521   128
# cfgP521CPc1pk     Yes  Yes  PK     SW  SW  PK    Prg     1        P-521   128
# cfgP521CPc2pk     Yes  Yes  PK     SW  SW  PK    Prg     2        P-521   128
# cfgP224IPc0cm     Yes  No   CM     n/a CM  SW    n/a     0        P-224   n/a
# cfgP224IPc1cm     Yes  No   CM     n/a CM  SW    n/a     1        P-224   n/a
# cfgP224IPc2cm     Yes  No   CM     n/a CM  SW    n/a     2        P-224   n/a
# cfgP224CPc0cm     Yes  Yes  CM     CM  CM  SW    Prg     0        P-224   128
# cfgP224CPc1cm     Yes  Yes  CM     CM  CM  SW    Prg     1        P-224   128
# cfgP224CPc2cm     Yes  Yes  CM     CM  CM  SW    Prg     2        P-224   128
# cfgP224CNc0cm     Yes  Yes  CM     CM  CM  SW    NVM     0        P-224   128
# cfgP224CNc1cm     Yes  Yes  CM     CM  CM  SW    NVM     1        P-224   128
# cfgP224CNc2cm     Yes  Yes  CM     CM  CM  SW    NVM     2        P-224   128
# cfgP224CXPc0cm    Yes  Yes  CM     CM  CM  SW    Prg     0        P-224   256
# cfgP224CXPc1cm    Yes  Yes  CM     CM  CM  SW    Prg     1        P-224   256
# cfgP224CXPc2cm    Yes  Yes  CM     CM  CM  SW    Prg     2        P-224   256
# cfgP224CXNc0cm    Yes  Yes  CM     CM  CM  SW    NVM     0        P-224   256
# cfgP224CXNc1cm    Yes  Yes  CM     CM  CM  SW    NVM     1        P-224   256
# cfgP224CXNc2cm    Yes  Yes  CM     CM  CM  SW    NVM     2        P-224   256
# cfgP224CXPKc0cm   Yes  Yes  CM     CM  CM  SW    Prg     0        P-224  K256
# cfgP224CXPKc1cm   Yes  Yes  CM     CM  CM  SW    Prg     1        P-224  K256
# cfgP224CXPKc2cm   Yes  Yes  CM     CM  CM  SW    Prg     2        P-224  K256
# cfgP224CXNKc0cm   Yes  Yes  CM     CM  CM  SW    NVM     0        P-224  K256
# cfgP224CXNKc1cm   Yes  Yes  CM     CM  CM  SW    NVM     1        P-224  K256
# cfgP224CXNKc2cm   Yes  Yes  CM     CM  CM  SW    NVM     2        P-224  K256
# cfgP256IPc0cm     Yes  No   CM     n/a CM  SW    n/a     0        P-256   n/a
# cfgP256IPc1cm     Yes  No   CM     n/a CM  SW    n/a     1        P-256   n/a
# cfgP256IPc2cm     Yes  No   CM     n/a CM  SW    n/a     2        P-256   n/a
# cfgP256CPc0cm     Yes  Yes  CM     CM  CM  SW    Prg     0        P-256   128
# cfgP256CPc1cm     Yes  Yes  CM     CM  CM  SW    Prg     1        P-256   128
# cfgP256CPc2cm     Yes  Yes  CM     CM  CM  SW    Prg     2        P-256   128
# cfgP256CNc0cm     Yes  Yes  CM     CM  CM  SW    NVM     0        P-256   128
# cfgP256CNc1cm     Yes  Yes  CM     CM  CM  SW    NVM     1        P-256   128
# cfgP256CNc2cm     Yes  Yes  CM     CM  CM  SW    NVM     2        P-256   128
# cfgP256CXPc0cm    Yes  Yes  CM     CM  CM  SW    Prg     0        P-256   256
# cfgP256CXPc1cm    Yes  Yes  CM     CM  CM  SW    Prg     1        P-256   256
# cfgP256CXPc2cm    Yes  Yes  CM     CM  CM  SW    Prg     2        P-256   256
# cfgP256CXNc0cm    Yes  Yes  CM     CM  CM  SW    NVM     0        P-256   256
# cfgP256CXNc1cm    Yes  Yes  CM     CM  CM  SW    NVM     1        P-256   256
# cfgP256CXNc2cm    Yes  Yes  CM     CM  CM  SW    NVM     2        P-256   256
# cfgP256CXPKc0cm   Yes  Yes  CM     CM  CM  SW    Prg     0        P-256  K256
# cfgP256CXPKc1cm   Yes  Yes  CM     CM  CM  SW    Prg     1        P-256  K256
# cfgP256CXPKc2cm   Yes  Yes  CM     CM  CM  SW    Prg     2        P-256  K256
# cfgP256CXNKc0cm   Yes  Yes  CM     CM  CM  SW    NVM     0        P-256  K256
# cfgP256CXNKc1cm   Yes  Yes  CM     CM  CM  SW    NVM     1        P-256  K256
# cfgP256CXNKc2cm   Yes  Yes  CM     CM  CM  SW    NVM     2        P-256  K256
# cfgP224IPc0cmpk   Yes  No   CM+PK  n/a CM  PK    n/a     0        P-224   n/a
# cfgP224IPc1cmpk   Yes  No   CM+PK  n/a CM  PK    n/a     1        P-224   n/a
# cfgP224IPc2cmpk   Yes  No   CM+PK  n/a CM  PK    n/a     2        P-224   n/a
# cfgP224CPc0cmpk   Yes  Yes  CM+PK  CM  CM  PK    Prg     0        P-224   128
# cfgP224CPc1cmpk   Yes  Yes  CM+PK  CM  CM  PK    Prg     1        P-224   128
# cfgP224CPc2cmpk   Yes  Yes  CM+PK  CM  CM  PK    Prg     2        P-224   128
# cfgP224CNc0cmpk   Yes  Yes  CM+PK  CM  CM  PK    NVM     0        P-224   128
# cfgP224CNc1cmpk   Yes  Yes  CM+PK  CM  CM  PK    NVM     1        P-224   128
# cfgP224CNc2cmpk   Yes  Yes  CM+PK  CM  CM  PK    NVM     2        P-224   128
# cfgP224CXPc0cmpk  Yes  Yes  CM+PK  CM  CM  PK    Prg     0        P-224   256
# cfgP224CXPc1cmpk  Yes  Yes  CM+PK  CM  CM  PK    Prg     1        P-224   256
# cfgP224CXPc2cmpk  Yes  Yes  CM+PK  CM  CM  PK    Prg     2        P-224   256
# cfgP224CXNc0cmpk  Yes  Yes  CM+PK  CM  CM  PK    NVM     0        P-224   256
# cfgP224CXNc1cmpk  Yes  Yes  CM+PK  CM  CM  PK    NVM     1        P-224   256
# cfgP224CXNc2cmpk  Yes  Yes  CM+PK  CM  CM  PK    NVM     2        P-224   256
# cfgP224CXPKc0cmpk Yes  Yes  CM+PK  CM  CM  PK    Prg     0        P-224  K256
# cfgP224CXPKc1cmpk Yes  Yes  CM+PK  CM  CM  PK    Prg     1        P-224  K256
# cfgP224CXPKc2cmpk Yes  Yes  CM+PK  CM  CM  PK    Prg     2        P-224  K256
# cfgP224CXNKc0cmpk Yes  Yes  CM+PK  CM  CM  PK    NVM     0        P-224  K256
# cfgP224CXNKc1cmpk Yes  Yes  CM+PK  CM  CM  PK    NVM     1        P-224  K256
# cfgP224CXNKc2cmpk Yes  Yes  CM+PK  CM  CM  PK    NVM     2        P-224  K256
# cfgP256IPc0cmpk   Yes  No   CM+PK  n/a CM  PK    n/a     0        P-256   n/a
# cfgP256IPc1cmpk   Yes  No   CM+PK  n/a CM  PK    n/a     1        P-256   n/a
# cfgP256IPc2cmpk   Yes  No   CM+PK  n/a CM  PK    n/a     2        P-256   n/a
# cfgP256CPc0cmpk   Yes  Yes  CM+PK  CM  CM  PK    Prg     0        P-256   128
# cfgP256CPc1cmpk   Yes  Yes  CM+PK  CM  CM  PK    Prg     1        P-256   128
# cfgP256CPc2cmpk   Yes  Yes  CM+PK  CM  CM  PK    Prg     2        P-256   128
# cfgP256CNc0cmpk   Yes  Yes  CM+PK  CM  CM  PK    NVM     0        P-256   128
# cfgP256CNc1cmpk   Yes  Yes  CM+PK  CM  CM  PK    NVM     1        P-256   128
# cfgP256CNc2cmpk   Yes  Yes  CM+PK  CM  CM  PK    NVM     2        P-256   128
# cfgP256CXPc0cmpk  Yes  Yes  CM+PK  CM  CM  PK    Prg     0        P-256   256
# cfgP256CXPc1cmpk  Yes  Yes  CM+PK  CM  CM  PK    Prg     1        P-256   256
# cfgP256CXPc2cmpk  Yes  Yes  CM+PK  CM  CM  PK    Prg     2        P-256   256
# cfgP256CXNc0cmpk  Yes  Yes  CM+PK  CM  CM  PK    NVM     0        P-256   256
# cfgP256CXNc1cmpk  Yes  Yes  CM+PK  CM  CM  PK    NVM     1        P-256   256
# cfgP256CXNc2cmpk  Yes  Yes  CM+PK  CM  CM  PK    NVM     2        P-256   256
# cfgP256CXPKc0cmpk Yes  Yes  CM+PK  CM  CM  PK    Prg     0        P-256  K256
# cfgP256CXPKc1cmpk Yes  Yes  CM+PK  CM  CM  PK    Prg     1        P-256  K256
# cfgP256CXPKc2cmpk Yes  Yes  CM+PK  CM  CM  PK    Prg     2        P-256  K256
# cfgP256CXNKc0cmpk Yes  Yes  CM+PK  CM  CM  PK    NVM     0        P-256  K256
# cfgP256CXNKc1cmpk Yes  Yes  CM+PK  CM  CM  PK    NVM     1        P-256  K256
# cfgP256CXNKc2cmpk Yes  Yes  CM+PK  CM  CM  PK    NVM     2        P-256  K256
# cfgP224IPc0pe     Yes  No   PE     n/a PE  SW    n/a     0        P-224   n/a
# cfgP224IPc1pe     Yes  No   PE     n/a PE  SW    n/a     1        P-224   n/a
# cfgP224IPc2pe     Yes  No   PE     n/a PE  SW    n/a     2        P-224   n/a
# cfgP224CPc0pe     Yes  Yes  PE     PE  PE  SW    Prg     0        P-224   128
# cfgP224CPc1pe     Yes  Yes  PE     PE  PE  SW    Prg     1        P-224   128
# cfgP224CPc2pe     Yes  Yes  PE     PE  PE  SW    Prg     2        P-224   128
# cfgP256IPc0pe     Yes  No   PE     n/a PE  SW    n/a     0        P-256   n/a
# cfgP256IPc1pe     Yes  No   PE     n/a PE  SW    n/a     1        P-256   n/a
# cfgP256IPc2pe     Yes  No   PE     n/a PE  SW    n/a     2        P-256   n/a
# cfgP256CPc0pe     Yes  Yes  PE     PE  PE  SW    Prg     0        P-256   128
# cfgP256CPc1pe     Yes  Yes  PE     PE  PE  SW    Prg     1        P-256   128
# cfgP256CPc2pe     Yes  Yes  PE     PE  PE  SW    Prg     2        P-256   128
# cfgP224IPc0pepk   Yes  No   PE+PK  n/a PE  PK    n/a     0        P-224   n/a
# cfgP224IPc1pepk   Yes  No   PE+PK  n/a PE  PK    n/a     1        P-224   n/a
# cfgP224IPc2pepk   Yes  No   PE+PK  n/a PE  PK    n/a     2        P-224   n/a
# cfgP224CPc0pepk   Yes  Yes  PE+PK  PE  PE  PK    Prg     0        P-224   128
# cfgP224CPc1pepk   Yes  Yes  PE+PK  PE  PE  PK    Prg     1        P-224   128
# cfgP224CPc2pepk   Yes  Yes  PE+PK  PE  PE  PK    Prg     2        P-224   128
# cfgP256IPc0pepk   Yes  No   PE+PK  n/a PE  PK    n/a     0        P-256   n/a
# cfgP256IPc1pepk   Yes  No   PE+PK  n/a PE  PK    n/a     1        P-256   n/a
# cfgP256IPc2pepk   Yes  No   PE+PK  n/a PE  PK    n/a     2        P-256   n/a
# cfgP256CPc0pepk   Yes  Yes  PE+PK  PE  PE  PK    Prg     0        P-256   128
# cfgP256CPc1pepk   Yes  Yes  PE+PK  PE  PE  PK    Prg     1        P-256   128
# cfgP256CPc2pepk   Yes  Yes  PE+PK  PE  PE  PK    Prg     2        P-256   128
#


ACLOCAL_AMFLAGS = -I ../../m4
AM_CFLAGS = -std=gnu99 -Wall -pedantic

top_src = $(top_srcdir)/../..

#
# Configuration Options:
#
# ENABLE_SBSW
#    Indicates that the software-only configuration is selected
# ENABLE_SBSM
#    Indicates that the Security Module (HW) configuration is selected for
#    symmetric and asymmetric crypto operations.
# ENABLE_SBPK
#    Indicates that the Public Key Accelerator/Processor (HW) configuration
#    is selected for asymmetric crypto operations and that the symmetric crypto
#    operations are performed via software.
# ENABLE_SBCM
#    Indicates that the Crypto Module (HW) configuration is selected for
#    symmetric crypto operations and that the asymmetric crypto operations are
#    performed via software.
# ENABLE_SBCMPK
#    Indicates that the Crypto Module (HW) configuration is selected for
#    symmetric crypto operations and the Public Key Accelerator/Processor (HW)
#    configuration for asymmetric crypto operations.
# ENABLE_SBPE
#    Indicates that the Packet Engine (HW) configuration is selected for
#    symmetric crypto operations and that the asymmetric crypto operations are
#    performed via software.
# ENABLE_SBPEPK
#    Indicates that the Packet Engine (HW) configuration is selected for
#    symmetric crypto operations and the Public Key Accelerator/Processor (HW)
#    configuration for asymmetric crypto operations.
# ENABLE_SBSIMPLE
#    Indicates that the software-only configuration is selected with simplified
#    and limited functionality.
#
# ENABLE_BARE
#    Indicates that the barebones-specific configuration is selected
# ENABLE_CUSTOM
#    Indicates that the customer-specific configuration is selected
# ENABLE_VERSATILE
#    Indicates that the Versatile configuration is selected
#
# Note: ENABLE_BARE/CUSTOM/VERSATILE are multiple-exclusive

#
# ENABLE_DEBUG
#    Enable debugging
# ENABLE_STACK
#    Enable stack tracing
# ENABLE_HEAP
#    Enable heap tracing
# ENABLE_PERF
#    Enable performance info
# ENABLE_COVERAGE
#    Enable coverage tracing
# ENABLE_POLLING
#    Enable polling instead of interrupt
# ENABLE_SBSIM_TIME
#    Enable sbsim_(decrypt_and_)verify time reporting
#


if ENABLE_BARE
CPPFLAGS += -DCFG_ENABLE_TARGET_BARE
ABSTRACTION_BASICDEFS := v4_bare
ABSTRACTION_CLIB := v4_bare
endif

if ENABLE_CUSTOM
CPPFLAGS += -DCFG_ENABLE_TARGET_CUSTOM
ABSTRACTION_BASICDEFS := v4_bare
ABSTRACTION_CLIB := v4_bare
endif

if ENABLE_VERSATILE
CPPFLAGS += -DCFG_ENABLE_TARGET_VERSATILE
ABSTRACTION_BASICDEFS := v4_safezone/Basic_Defs
ABSTRACTION_CLIB := v4_safezone/CLib_Abstraction
endif



ABSTRACTION_BASICDEFS ?= v4/Basic_Defs
ABSTRACTION_CLIB ?= v4/CLib_Abstraction

if ENABLE_STACK
CFLAGS += -finstrument-functions
CPPFLAGS += -DSTACK_MEASUREMENT -DCFG_ENABLE_STACK
endif

if ENABLE_HEAP
CPPFLAGS += -DHEAP_MEASUREMENT -DCFG_ENABLE_HEAP
endif

if ENABLE_SBSIM_TIME
CPPFLAGS += -DSBSIM_TIME_INFO
endif

if ENABLE_GCC_STRICT_WARNINGS
AM_CFLAGS += -W -Wshadow -Wvla
endif

if ENABLE_DEBUG
# Debugging enabled.
CPPFLAGS += -DCFG_ENABLE_DEBUG
else
# No debugging (performance build)
CPPFLAGS += -DCFG_IMPLDEFS_NO_DEBUG
endif

# libdebug.a is unconditionally required when building Secure Boot
# with C-models.
OPT_LIBDEBUG_A = libdebug.a

if ENABLE_COVERAGE
CPPFLAGS += -DGCOV_PROFILE -DCFG_ENABLE_COVERAGE
LIBS += -lgcov
if ENABLE_DEBUG
# With debugging, test coverage analysis requires enabling additional flag to
# dump coverage reports prior abnormal exit
CPPFLAGS += -DDEBUG_CF_ABORT_WRITE_PROFILE
endif
endif


CONFIGURATION_INCLUDES = \
    -I$(top_src)/SBLib/Config \
    -I$(top_src)/Config


if ENABLE_BARE
DMARES_INCLUDE = -I$(top_src)/Integration/DMARes_Record_bare/incl
else
DMARES_INCLUDE = -I$(top_src)/Integration/DMARes_Record/incl
endif

FRAMEWORK_HEADER_LIST = \
    $(top_src)/Framework/PUBDEFS/incl/public_defs.h

include_HEADERS =

# Flags for hardware assisted compilations
LIBSB_GENERAL_HW =





SECUREBOOTSIM_PROGRAM_NAMES_S = \
    securebootsim_cfgP224IPc0s \
    securebootsim_cfgP224IPc1s \
    securebootsim_cfgP224IPc2s \
    securebootsim_cfgP224CPc0s \
    securebootsim_cfgP224CPc1s \
    securebootsim_cfgP224CPc2s \
    securebootsim_cfgP256IPc0s \
    securebootsim_cfgP256IPc1s \
    securebootsim_cfgP256IPc2s \
    securebootsim_cfgP256CPc0s \
    securebootsim_cfgP256CPc1s \
    securebootsim_cfgP256CPc2s

SECUREBOOTSIM_PROGRAM_NAMES_SW = \
    securebootsim_cfgP224IPc0sw \
    securebootsim_cfgP224IPc1sw \
    securebootsim_cfgP224IPc2sw \
    securebootsim_cfgP224CPc0sw \
    securebootsim_cfgP224CPc1sw \
    securebootsim_cfgP224CPc2sw \
    securebootsim_cfgP224CXPc0sw \
    securebootsim_cfgP224CXPc1sw \
    securebootsim_cfgP224CXPc2sw \
    securebootsim_cfgP224CXPKc0sw \
    securebootsim_cfgP224CXPKc1sw \
    securebootsim_cfgP224CXPKc2sw \
    securebootsim_cfgP256IPc0sw \
    securebootsim_cfgP256IPc1sw \
    securebootsim_cfgP256IPc2sw \
    securebootsim_cfgP256CPc0sw \
    securebootsim_cfgP256CPc1sw \
    securebootsim_cfgP256CPc2sw \
    securebootsim_cfgP256CXPc0sw \
    securebootsim_cfgP256CXPc1sw \
    securebootsim_cfgP256CXPc2sw \
    securebootsim_cfgP256CXPKc0sw \
    securebootsim_cfgP256CXPKc1sw \
    securebootsim_cfgP256CXPKc2sw \
    securebootsim_cfgP384IPc0sw \
    securebootsim_cfgP384IPc1sw \
    securebootsim_cfgP384IPc2sw \
    securebootsim_cfgP384CPc0sw \
    securebootsim_cfgP384CPc1sw \
    securebootsim_cfgP384CPc2sw \
    securebootsim_cfgP384CXPc0sw \
    securebootsim_cfgP384CXPc1sw \
    securebootsim_cfgP384CXPc2sw \
    securebootsim_cfgP384CXPKc0sw \
    securebootsim_cfgP384CXPKc1sw \
    securebootsim_cfgP384CXPKc2sw \
    securebootsim_cfgP521IPc0sw \
    securebootsim_cfgP521IPc1sw \
    securebootsim_cfgP521IPc2sw \
    securebootsim_cfgP521CPc0sw \
    securebootsim_cfgP521CPc1sw \
    securebootsim_cfgP521CPc2sw \
    securebootsim_cfgP521CXPc0sw \
    securebootsim_cfgP521CXPc1sw \
    securebootsim_cfgP521CXPc2sw \
    securebootsim_cfgP521CXPKc0sw \
    securebootsim_cfgP521CXPKc1sw \
    securebootsim_cfgP521CXPKc2sw

SECUREBOOTSIM_PROGRAM_NAMES_SM = \
    securebootsim_cfgP224IPc0sm \
    securebootsim_cfgP224IPc1sm \
    securebootsim_cfgP224IPc2sm \
    securebootsim_cfgP224CPc0sm \
    securebootsim_cfgP224CPc1sm \
    securebootsim_cfgP224CPc2sm \
    securebootsim_cfgP224CNc0sm \
    securebootsim_cfgP224CNc1sm \
    securebootsim_cfgP224CNc2sm \
    securebootsim_cfgP224CXPc0sm \
    securebootsim_cfgP224CXPc1sm \
    securebootsim_cfgP224CXPc2sm \
    securebootsim_cfgP224CXPKc0sm \
    securebootsim_cfgP224CXPKc1sm \
    securebootsim_cfgP224CXPKc2sm \
    securebootsim_cfgP224CXNc0sm \
    securebootsim_cfgP224CXNc1sm \
    securebootsim_cfgP224CXNc2sm \
    securebootsim_cfgP224CXNKc0sm \
    securebootsim_cfgP224CXNKc1sm \
    securebootsim_cfgP224CXNKc2sm \
    securebootsim_cfgP256IPc0sm \
    securebootsim_cfgP256IPc1sm \
    securebootsim_cfgP256IPc2sm \
    securebootsim_cfgP256CPc0sm \
    securebootsim_cfgP256CPc1sm \
    securebootsim_cfgP256CPc2sm \
    securebootsim_cfgP256CNc0sm \
    securebootsim_cfgP256CNc1sm \
    securebootsim_cfgP256CNc2sm \
    securebootsim_cfgP256CXPc0sm \
    securebootsim_cfgP256CXPc1sm \
    securebootsim_cfgP256CXPc2sm \
    securebootsim_cfgP256CXPKc0sm \
    securebootsim_cfgP256CXPKc1sm \
    securebootsim_cfgP256CXPKc2sm \
    securebootsim_cfgP256CXNc0sm \
    securebootsim_cfgP256CXNc1sm \
    securebootsim_cfgP256CXNc2sm \
    securebootsim_cfgP256CXNKc0sm \
    securebootsim_cfgP256CXNKc1sm \
    securebootsim_cfgP256CXNKc2sm \
    securebootsim_cfgP384IPc0sm \
    securebootsim_cfgP384IPc1sm \
    securebootsim_cfgP384IPc2sm \
    securebootsim_cfgP384CPc0sm \
    securebootsim_cfgP384CPc1sm \
    securebootsim_cfgP384CPc2sm \
    securebootsim_cfgP384CNc0sm \
    securebootsim_cfgP384CNc1sm \
    securebootsim_cfgP384CNc2sm \
    securebootsim_cfgP384CXPc0sm \
    securebootsim_cfgP384CXPc1sm \
    securebootsim_cfgP384CXPc2sm \
    securebootsim_cfgP384CXPKc0sm \
    securebootsim_cfgP384CXPKc1sm \
    securebootsim_cfgP384CXPKc2sm \
    securebootsim_cfgP384CXNc0sm \
    securebootsim_cfgP384CXNc1sm \
    securebootsim_cfgP384CXNc2sm \
    securebootsim_cfgP384CXNKc0sm \
    securebootsim_cfgP384CXNKc1sm \
    securebootsim_cfgP384CXNKc2sm \
    securebootsim_cfgP521IPc0sm \
    securebootsim_cfgP521IPc1sm \
    securebootsim_cfgP521IPc2sm \
    securebootsim_cfgP521CPc0sm \
    securebootsim_cfgP521CPc1sm \
    securebootsim_cfgP521CPc2sm \
    securebootsim_cfgP521CNc0sm \
    securebootsim_cfgP521CNc1sm \
    securebootsim_cfgP521CNc2sm \
    securebootsim_cfgP521CXPc0sm \
    securebootsim_cfgP521CXPc1sm \
    securebootsim_cfgP521CXPc2sm \
    securebootsim_cfgP521CXPKc0sm \
    securebootsim_cfgP521CXPKc1sm \
    securebootsim_cfgP521CXPKc2sm \
    securebootsim_cfgP521CXNc0sm \
    securebootsim_cfgP521CXNc1sm \
    securebootsim_cfgP521CXNc2sm \
    securebootsim_cfgP521CXNKc0sm \
    securebootsim_cfgP521CXNKc1sm \
    securebootsim_cfgP521CXNKc2sm

SECUREBOOTSIM_PROGRAM_NAMES_CM = \
    securebootsim_cfgP224IPc0cm \
    securebootsim_cfgP224IPc1cm \
    securebootsim_cfgP224IPc2cm \
    securebootsim_cfgP224CPc0cm \
    securebootsim_cfgP224CPc1cm \
    securebootsim_cfgP224CPc2cm \
    securebootsim_cfgP224CNc0cm \
    securebootsim_cfgP224CNc1cm \
    securebootsim_cfgP224CNc2cm \
    securebootsim_cfgP224CXPc0cm \
    securebootsim_cfgP224CXPc1cm \
    securebootsim_cfgP224CXPc2cm \
    securebootsim_cfgP224CXPKc0cm \
    securebootsim_cfgP224CXPKc1cm \
    securebootsim_cfgP224CXPKc2cm \
    securebootsim_cfgP224CXNc0cm \
    securebootsim_cfgP224CXNc1cm \
    securebootsim_cfgP224CXNc2cm \
    securebootsim_cfgP224CXNKc0cm \
    securebootsim_cfgP224CXNKc1cm \
    securebootsim_cfgP224CXNKc2cm \
    securebootsim_cfgP256IPc0cm \
    securebootsim_cfgP256IPc1cm \
    securebootsim_cfgP256IPc2cm \
    securebootsim_cfgP256CPc0cm \
    securebootsim_cfgP256CPc1cm \
    securebootsim_cfgP256CPc2cm \
    securebootsim_cfgP256CNc0cm \
    securebootsim_cfgP256CNc1cm \
    securebootsim_cfgP256CNc2cm \
    securebootsim_cfgP256CXPc0cm \
    securebootsim_cfgP256CXPc1cm \
    securebootsim_cfgP256CXPc2cm \
    securebootsim_cfgP256CXPKc0cm \
    securebootsim_cfgP256CXPKc1cm \
    securebootsim_cfgP256CXPKc2cm \
    securebootsim_cfgP256CXNc0cm \
    securebootsim_cfgP256CXNc1cm \
    securebootsim_cfgP256CXNc2cm \
    securebootsim_cfgP256CXNKc0cm \
    securebootsim_cfgP256CXNKc1cm \
    securebootsim_cfgP256CXNKc2cm

SECUREBOOTSIM_PROGRAM_NAMES_CMPK = \
    securebootsim_cfgP224IPc0cmpk \
    securebootsim_cfgP224IPc1cmpk \
    securebootsim_cfgP224IPc2cmpk \
    securebootsim_cfgP224CPc0cmpk \
    securebootsim_cfgP224CPc1cmpk \
    securebootsim_cfgP224CPc2cmpk \
    securebootsim_cfgP224CNc0cmpk \
    securebootsim_cfgP224CNc1cmpk \
    securebootsim_cfgP224CNc2cmpk \
    securebootsim_cfgP224CXPc0cmpk \
    securebootsim_cfgP224CXPc1cmpk \
    securebootsim_cfgP224CXPc2cmpk \
    securebootsim_cfgP224CXPKc0cmpk \
    securebootsim_cfgP224CXPKc1cmpk \
    securebootsim_cfgP224CXPKc2cmpk \
    securebootsim_cfgP224CXNc0cmpk \
    securebootsim_cfgP224CXNc1cmpk \
    securebootsim_cfgP224CXNc2cmpk \
    securebootsim_cfgP224CXNKc0cmpk \
    securebootsim_cfgP224CXNKc1cmpk \
    securebootsim_cfgP224CXNKc2cmpk \
    securebootsim_cfgP256IPc0cmpk \
    securebootsim_cfgP256IPc1cmpk \
    securebootsim_cfgP256IPc2cmpk \
    securebootsim_cfgP256CPc0cmpk \
    securebootsim_cfgP256CPc1cmpk \
    securebootsim_cfgP256CPc2cmpk \
    securebootsim_cfgP256CNc0cmpk \
    securebootsim_cfgP256CNc1cmpk \
    securebootsim_cfgP256CNc2cmpk \
    securebootsim_cfgP256CXPc0cmpk \
    securebootsim_cfgP256CXPc1cmpk \
    securebootsim_cfgP256CXPc2cmpk \
    securebootsim_cfgP256CXPKc0cmpk \
    securebootsim_cfgP256CXPKc1cmpk \
    securebootsim_cfgP256CXPKc2cmpk \
    securebootsim_cfgP256CXNc0cmpk \
    securebootsim_cfgP256CXNc1cmpk \
    securebootsim_cfgP256CXNc2cmpk \
    securebootsim_cfgP256CXNKc0cmpk \
    securebootsim_cfgP256CXNKc1cmpk \
    securebootsim_cfgP256CXNKc2cmpk

SECUREBOOTSIM_PROGRAM_NAMES_PE = \
    securebootsim_cfgP224IPc0pe \
    securebootsim_cfgP224IPc1pe \
    securebootsim_cfgP224IPc2pe \
    securebootsim_cfgP224CPc0pe \
    securebootsim_cfgP224CPc1pe \
    securebootsim_cfgP224CPc2pe \
    securebootsim_cfgP256IPc0pe \
    securebootsim_cfgP256IPc1pe \
    securebootsim_cfgP256IPc2pe \
    securebootsim_cfgP256CPc0pe \
    securebootsim_cfgP256CPc1pe \
    securebootsim_cfgP256CPc2pe

SECUREBOOTSIM_PROGRAM_NAMES_PEPK = \
    securebootsim_cfgP224IPc0pepk \
    securebootsim_cfgP224IPc1pepk \
    securebootsim_cfgP224IPc2pepk \
    securebootsim_cfgP224CPc0pepk \
    securebootsim_cfgP224CPc1pepk \
    securebootsim_cfgP224CPc2pepk \
    securebootsim_cfgP256IPc0pepk \
    securebootsim_cfgP256IPc1pepk \
    securebootsim_cfgP256IPc2pepk \
    securebootsim_cfgP256CPc0pepk \
    securebootsim_cfgP256CPc1pepk \
    securebootsim_cfgP256CPc2pepk

SECUREBOOTSIM_PROGRAM_NAMES_PK = \
    securebootsim_cfgP224IPc0pk \
    securebootsim_cfgP224IPc1pk \
    securebootsim_cfgP224IPc2pk \
    securebootsim_cfgP224CPc0pk \
    securebootsim_cfgP224CPc1pk \
    securebootsim_cfgP224CPc2pk \
    securebootsim_cfgP256IPc0pk \
    securebootsim_cfgP256IPc1pk \
    securebootsim_cfgP256IPc2pk \
    securebootsim_cfgP256CPc0pk \
    securebootsim_cfgP256CPc1pk \
    securebootsim_cfgP256CPc2pk \
    securebootsim_cfgP384IPc0pk \
    securebootsim_cfgP384IPc1pk \
    securebootsim_cfgP384IPc2pk \
    securebootsim_cfgP384CPc0pk \
    securebootsim_cfgP384CPc1pk \
    securebootsim_cfgP384CPc2pk \
    securebootsim_cfgP521IPc0pk \
    securebootsim_cfgP521IPc1pk \
    securebootsim_cfgP521IPc2pk \
    securebootsim_cfgP521CPc0pk \
    securebootsim_cfgP521CPc1pk \
    securebootsim_cfgP521CPc2pk

# Built by default
bin_PROGRAMS =

# Not installed by default
noinst_PROGRAMS =

# These targets are built for testing.
check_PROGRAMS =

# These targets are not build by default.
EXTRA_PROGRAMS = \
    sbsmtest224 \
    sbcmtest224 sbcmpktest224 \
    sbpetest224 sbpepktest224 \
    sbpktest224 \
    sbstest224

# By default the secureboot simulator uses securebootsim_cfgP224CPc2sw
SECUREBOOTSIM = securebootsim_cfgP224CPc2sw

# Add selected SW/HW variants to list of files to build.
if ENABLE_SBSW
bin_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_SW) updatetool securebootsim sbtest_publicdata_sw
else
EXTRA_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_SW) updatetool sbtest_publicdata_sw
endif

if ENABLE_SBSM
bin_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_SM) securebootsim sbtest_publicdata_sm
SECUREBOOTSIM = securebootsim_cfgP224CPc2sm
else
EXTRA_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_SM) sbtest_publicdata_sm
endif

if ENABLE_SBCM
bin_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_CM) securebootsim sbtest_publicdata_cm
SECUREBOOTSIM = securebootsim_cfgP224CPc2cm
LIBSBPD_CM_A = libsb_cfgP224IPc0cm.a
else
if ENABLE_SBCMPK
EXTRA_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_CM)
else
EXTRA_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_CM) sbtest_publicdata_cm
LIBSBPD_CM_A =
endif
endif

if ENABLE_SBCMPK
bin_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_CMPK) securebootsim sbtest_publicdata_cm
SECUREBOOTSIM = securebootsim_cfgP224CPc2cmpk
LIBSBPD_CM_A = libsb_cfgP224IPc0cmpk.a
else
EXTRA_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_CMPK)
endif

if ENABLE_SBPE
bin_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_PE) securebootsim
SECUREBOOTSIM = securebootsim_cfgP224CPc2pe
else
EXTRA_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_PE)
endif

if ENABLE_SBPEPK
bin_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_PEPK) securebootsim
SECUREBOOTSIM = securebootsim_cfgP224CPc2pepk
else
EXTRA_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_PEPK)
endif

if ENABLE_SBPK
bin_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_PK) securebootsim
SECUREBOOTSIM = securebootsim_cfgP224CPc2pk
else
EXTRA_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_PK)
endif

if ENABLE_SBSIMPLE
bin_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_S) securebootsim
SECUREBOOTSIM = securebootsim_cfgP224CPc0s
else
EXTRA_PROGRAMS += $(SECUREBOOTSIM_PROGRAM_NAMES_S)
endif

# Convenience targets for building selected subsets of Secure Boot targets.
# (Note: these override --enable/disable-sb* configuration)
all_hw: all_s all_sw all_sm all_cm all_cmpk all_pe all_pepk all_pk securebootsim

usual: all_s \
    securebootsim_cfgP224IPc2sw \
    securebootsim_cfgP224CPc2sw \
    securebootsim_cfgP224IPc2sm \
    securebootsim_cfgP224CPc2sm \
    securebootsim_cfgP224IPc2cm \
    securebootsim_cfgP224CPc2cm \
    securebootsim_cfgP224IPc2cmpk \
    securebootsim_cfgP224CPc2cmpk \
    securebootsim_cfgP224IPc2pk \
    securebootsim_cfgP224CPc2pk \
    securebootsim_cfgP224IPc2pe \
    securebootsim_cfgP224CPc2pe \
    securebootsim_cfgP224IPc2pepk \
    securebootsim_cfgP224CPc2pepk \
    securebootsim_cfgP224IPc2s \
    securebootsim_cfgP224CPc2s \
    securebootsim_cfgP224CXPc2sw \
    securebootsim_cfgP224CXPKc2sw \
    securebootsim_cfgP224CNc2sm \
    securebootsim_cfgP224CXPc2sm \
    securebootsim_cfgP224CXPKc2sm \
    securebootsim_cfgP224CXNc2sm \
    securebootsim_cfgP224CXNKc2sm \
    securebootsim_cfgP224CNc2cm \
    securebootsim_cfgP224CXPc2cm \
    securebootsim_cfgP224CXPKc2cm \
    securebootsim_cfgP224CXNc2cm \
    securebootsim_cfgP224CXNKc2cm \
    securebootsim_cfgP224CNc2cmpk \
    securebootsim_cfgP224CXPc2cmpk \
    securebootsim_cfgP224CXPKc2cmpk \
    securebootsim_cfgP224CXNc2cmpk \
    securebootsim_cfgP224CXNKc2cmpk \
    securebootsim_cfgP256IPc2sw \
    securebootsim_cfgP256CPc2sw \
    securebootsim_cfgP256IPc2sm \
    securebootsim_cfgP256CPc2sm \
    securebootsim_cfgP256IPc2cm \
    securebootsim_cfgP256CPc2cm \
    securebootsim_cfgP256IPc2cmpk \
    securebootsim_cfgP256CPc2cmpk \
    securebootsim_cfgP256IPc2pk \
    securebootsim_cfgP256CPc2pk \
    securebootsim_cfgP256IPc2pe \
    securebootsim_cfgP256CPc2pe \
    securebootsim_cfgP256IPc2pepk \
    securebootsim_cfgP256CPc2pepk \
    securebootsim_cfgP256IPc2s \
    securebootsim_cfgP256CPc2s \
    securebootsim_cfgP256CXPc2sw \
    securebootsim_cfgP256CXPKc2sw \
    securebootsim_cfgP256CNc2sm \
    securebootsim_cfgP256CXPc2sm \
    securebootsim_cfgP256CXPKc2sm \
    securebootsim_cfgP256CXNc2sm \
    securebootsim_cfgP256CXNKc2sm \
    securebootsim_cfgP256CNc2cm \
    securebootsim_cfgP256CXPc2cm \
    securebootsim_cfgP256CXPKc2cm \
    securebootsim_cfgP256CXNc2cm \
    securebootsim_cfgP256CXNKc2cm \
    securebootsim_cfgP256CNc2cmpk \
    securebootsim_cfgP256CXPc2cmpk \
    securebootsim_cfgP256CXPKc2cmpk \
    securebootsim_cfgP256CXNc2cmpk \
    securebootsim_cfgP256CXNKc2cmpk \
    securebootsim_cfgP384IPc2sw \
    securebootsim_cfgP384CPc2sw \
    securebootsim_cfgP384IPc2sm \
    securebootsim_cfgP384CPc2sm \
    securebootsim_cfgP384IPc2pk \
    securebootsim_cfgP384CPc2pk \
    securebootsim_cfgP384CXPc2sw \
    securebootsim_cfgP384CXPKc2sw \
    securebootsim_cfgP384CNc2sm \
    securebootsim_cfgP384CXPc2sm \
    securebootsim_cfgP384CXPKc2sm \
    securebootsim_cfgP384CXNc2sm \
    securebootsim_cfgP384CXNKc2sm \
    securebootsim_cfgP521IPc2sw \
    securebootsim_cfgP521CPc2sw \
    securebootsim_cfgP521IPc2sm \
    securebootsim_cfgP521CPc2sm \
    securebootsim_cfgP521IPc2pk \
    securebootsim_cfgP521CPc2pk \
    securebootsim_cfgP521CXPc2sw \
    securebootsim_cfgP521CXPKc2sw \
    securebootsim_cfgP521CNc2sm \
    securebootsim_cfgP521CXPc2sm \
    securebootsim_cfgP521CXPKc2sm \
    securebootsim_cfgP521CXNc2sm \
    securebootsim_cfgP521CXNKc2sm

all_sm: $(SECUREBOOTSIM_PROGRAM_NAMES_SM)
all_cm: $(SECUREBOOTSIM_PROGRAM_NAMES_CM)
all_cmpk: $(SECUREBOOTSIM_PROGRAM_NAMES_CMPK)
all_pe: $(SECUREBOOTSIM_PROGRAM_NAMES_PE)
all_pepk: $(SECUREBOOTSIM_PROGRAM_NAMES_PEPK)
all_pk: $(SECUREBOOTSIM_PROGRAM_NAMES_PK)
all_sw: $(SECUREBOOTSIM_PROGRAM_NAMES_SW)
all_s: $(SECUREBOOTSIM_PROGRAM_NAMES_S)

noinst_LIBRARIES =

EXTRA_LIBRARIES = \
    $(SB_SW_TEST_LIBRARIES_LIST) \
    libsb_cfgP224IPc0sw.a \
    libsb_cfgP224IPc1sw.a \
    libsb_cfgP224IPc2sw.a \
    libsb_cfgP224CPc0sw.a \
    libsb_cfgP224CPc1sw.a \
    libsb_cfgP224CPc2sw.a \
    libsb_cfgP224IPc0sm.a \
    libsb_cfgP224IPc1sm.a \
    libsb_cfgP224IPc2sm.a \
    libsb_cfgP224CPc0sm.a \
    libsb_cfgP224CPc1sm.a \
    libsb_cfgP224CPc2sm.a \
    libsb_cfgP224IPc0cm.a \
    libsb_cfgP224IPc1cm.a \
    libsb_cfgP224IPc2cm.a \
    libsb_cfgP224CPc0cm.a \
    libsb_cfgP224CPc1cm.a \
    libsb_cfgP224CPc2cm.a \
    libsb_cfgP224IPc0cmpk.a \
    libsb_cfgP224IPc1cmpk.a \
    libsb_cfgP224IPc2cmpk.a \
    libsb_cfgP224CPc0cmpk.a \
    libsb_cfgP224CPc1cmpk.a \
    libsb_cfgP224CPc2cmpk.a \
    libsb_cfgP224IPc0pk.a \
    libsb_cfgP224IPc1pk.a \
    libsb_cfgP224IPc2pk.a \
    libsb_cfgP224CPc0pk.a \
    libsb_cfgP224CPc1pk.a \
    libsb_cfgP224CPc2pk.a \
    libsb_cfgP224IPc0pe.a \
    libsb_cfgP224IPc1pe.a \
    libsb_cfgP224IPc2pe.a \
    libsb_cfgP224CPc0pe.a \
    libsb_cfgP224CPc1pe.a \
    libsb_cfgP224CPc2pe.a \
    libsb_cfgP224IPc0pepk.a \
    libsb_cfgP224IPc1pepk.a \
    libsb_cfgP224IPc2pepk.a \
    libsb_cfgP224CPc0pepk.a \
    libsb_cfgP224CPc1pepk.a \
    libsb_cfgP224CPc2pepk.a \
    libsb_cfgP224IPc0s.a \
    libsb_cfgP224IPc1s.a \
    libsb_cfgP224IPc2s.a \
    libsb_cfgP224CPc0s.a \
    libsb_cfgP224CPc1s.a \
    libsb_cfgP224CPc2s.a \
    libsb_cfgP224CXPc0sw.a \
    libsb_cfgP224CXPc1sw.a \
    libsb_cfgP224CXPc2sw.a \
    libsb_cfgP224CXPKc0sw.a \
    libsb_cfgP224CXPKc1sw.a \
    libsb_cfgP224CXPKc2sw.a \
    libsb_cfgP224CNc0sm.a \
    libsb_cfgP224CNc1sm.a \
    libsb_cfgP224CNc2sm.a \
    libsb_cfgP224CXPc0sm.a \
    libsb_cfgP224CXPc1sm.a \
    libsb_cfgP224CXPc2sm.a \
    libsb_cfgP224CXPKc0sm.a \
    libsb_cfgP224CXPKc1sm.a \
    libsb_cfgP224CXPKc2sm.a \
    libsb_cfgP224CXNc0sm.a \
    libsb_cfgP224CXNc1sm.a \
    libsb_cfgP224CXNc2sm.a \
    libsb_cfgP224CXNKc0sm.a \
    libsb_cfgP224CXNKc1sm.a \
    libsb_cfgP224CXNKc2sm.a \
    libsb_cfgP224CNc0cm.a \
    libsb_cfgP224CNc1cm.a \
    libsb_cfgP224CNc2cm.a \
    libsb_cfgP224CXPc0cm.a \
    libsb_cfgP224CXPc1cm.a \
    libsb_cfgP224CXPc2cm.a \
    libsb_cfgP224CXPKc0cm.a \
    libsb_cfgP224CXPKc1cm.a \
    libsb_cfgP224CXPKc2cm.a \
    libsb_cfgP224CXNc0cm.a \
    libsb_cfgP224CXNc1cm.a \
    libsb_cfgP224CXNc2cm.a \
    libsb_cfgP224CXNKc0cm.a \
    libsb_cfgP224CXNKc1cm.a \
    libsb_cfgP224CXNKc2cm.a \
    libsb_cfgP224CNc0cmpk.a \
    libsb_cfgP224CNc1cmpk.a \
    libsb_cfgP224CNc2cmpk.a \
    libsb_cfgP224CXPc0cmpk.a \
    libsb_cfgP224CXPc1cmpk.a \
    libsb_cfgP224CXPc2cmpk.a \
    libsb_cfgP224CXPKc0cmpk.a \
    libsb_cfgP224CXPKc1cmpk.a \
    libsb_cfgP224CXPKc2cmpk.a \
    libsb_cfgP224CXNc0cmpk.a \
    libsb_cfgP224CXNc1cmpk.a \
    libsb_cfgP224CXNc2cmpk.a \
    libsb_cfgP224CXNKc0cmpk.a \
    libsb_cfgP224CXNKc1cmpk.a \
    libsb_cfgP224CXNKc2cmpk.a \
    libsb_cfgP256IPc0sw.a \
    libsb_cfgP256IPc1sw.a \
    libsb_cfgP256IPc2sw.a \
    libsb_cfgP256CPc0sw.a \
    libsb_cfgP256CPc1sw.a \
    libsb_cfgP256CPc2sw.a \
    libsb_cfgP256IPc0sm.a \
    libsb_cfgP256IPc1sm.a \
    libsb_cfgP256IPc2sm.a \
    libsb_cfgP256CPc0sm.a \
    libsb_cfgP256CPc1sm.a \
    libsb_cfgP256CPc2sm.a \
    libsb_cfgP256IPc0cm.a \
    libsb_cfgP256IPc1cm.a \
    libsb_cfgP256IPc2cm.a \
    libsb_cfgP256CPc0cm.a \
    libsb_cfgP256CPc1cm.a \
    libsb_cfgP256CPc2cm.a \
    libsb_cfgP256IPc0cmpk.a \
    libsb_cfgP256IPc1cmpk.a \
    libsb_cfgP256IPc2cmpk.a \
    libsb_cfgP256CPc0cmpk.a \
    libsb_cfgP256CPc1cmpk.a \
    libsb_cfgP256CPc2cmpk.a \
    libsb_cfgP256IPc0pk.a \
    libsb_cfgP256IPc1pk.a \
    libsb_cfgP256IPc2pk.a \
    libsb_cfgP256CPc0pk.a \
    libsb_cfgP256CPc1pk.a \
    libsb_cfgP256CPc2pk.a \
    libsb_cfgP256IPc0pe.a \
    libsb_cfgP256IPc1pe.a \
    libsb_cfgP256IPc2pe.a \
    libsb_cfgP256CPc0pe.a \
    libsb_cfgP256CPc1pe.a \
    libsb_cfgP256CPc2pe.a \
    libsb_cfgP256IPc0pepk.a \
    libsb_cfgP256IPc1pepk.a \
    libsb_cfgP256IPc2pepk.a \
    libsb_cfgP256CPc0pepk.a \
    libsb_cfgP256CPc1pepk.a \
    libsb_cfgP256CPc2pepk.a \
    libsb_cfgP256IPc0s.a \
    libsb_cfgP256IPc1s.a \
    libsb_cfgP256IPc2s.a \
    libsb_cfgP256CPc0s.a \
    libsb_cfgP256CPc1s.a \
    libsb_cfgP256CPc2s.a \
    libsb_cfgP256CXPc0sw.a \
    libsb_cfgP256CXPc1sw.a \
    libsb_cfgP256CXPc2sw.a \
    libsb_cfgP256CXPKc0sw.a \
    libsb_cfgP256CXPKc1sw.a \
    libsb_cfgP256CXPKc2sw.a \
    libsb_cfgP256CNc0sm.a \
    libsb_cfgP256CNc1sm.a \
    libsb_cfgP256CNc2sm.a \
    libsb_cfgP256CXPc0sm.a \
    libsb_cfgP256CXPc1sm.a \
    libsb_cfgP256CXPc2sm.a \
    libsb_cfgP256CXPKc0sm.a \
    libsb_cfgP256CXPKc1sm.a \
    libsb_cfgP256CXPKc2sm.a \
    libsb_cfgP256CXNc0sm.a \
    libsb_cfgP256CXNc1sm.a \
    libsb_cfgP256CXNc2sm.a \
    libsb_cfgP256CXNKc0sm.a \
    libsb_cfgP256CXNKc1sm.a \
    libsb_cfgP256CXNKc2sm.a \
    libsb_cfgP256CNc0cm.a \
    libsb_cfgP256CNc1cm.a \
    libsb_cfgP256CNc2cm.a \
    libsb_cfgP256CXPc0cm.a \
    libsb_cfgP256CXPc1cm.a \
    libsb_cfgP256CXPc2cm.a \
    libsb_cfgP256CXPKc0cm.a \
    libsb_cfgP256CXPKc1cm.a \
    libsb_cfgP256CXPKc2cm.a \
    libsb_cfgP256CXNc0cm.a \
    libsb_cfgP256CXNc1cm.a \
    libsb_cfgP256CXNc2cm.a \
    libsb_cfgP256CXNKc0cm.a \
    libsb_cfgP256CXNKc1cm.a \
    libsb_cfgP256CXNKc2cm.a \
    libsb_cfgP256CNc0cmpk.a \
    libsb_cfgP256CNc1cmpk.a \
    libsb_cfgP256CNc2cmpk.a \
    libsb_cfgP256CXPc0cmpk.a \
    libsb_cfgP256CXPc1cmpk.a \
    libsb_cfgP256CXPc2cmpk.a \
    libsb_cfgP256CXPKc0cmpk.a \
    libsb_cfgP256CXPKc1cmpk.a \
    libsb_cfgP256CXPKc2cmpk.a \
    libsb_cfgP256CXNc0cmpk.a \
    libsb_cfgP256CXNc1cmpk.a \
    libsb_cfgP256CXNc2cmpk.a \
    libsb_cfgP256CXNKc0cmpk.a \
    libsb_cfgP256CXNKc1cmpk.a \
    libsb_cfgP256CXNKc2cmpk.a \
    libsb_cfgP384IPc0sw.a \
    libsb_cfgP384IPc1sw.a \
    libsb_cfgP384IPc2sw.a \
    libsb_cfgP384CPc0sw.a \
    libsb_cfgP384CPc1sw.a \
    libsb_cfgP384CPc2sw.a \
    libsb_cfgP384IPc0sm.a \
    libsb_cfgP384IPc1sm.a \
    libsb_cfgP384IPc2sm.a \
    libsb_cfgP384CPc0sm.a \
    libsb_cfgP384CPc1sm.a \
    libsb_cfgP384CPc2sm.a \
    libsb_cfgP384IPc0pk.a \
    libsb_cfgP384IPc1pk.a \
    libsb_cfgP384IPc2pk.a \
    libsb_cfgP384CPc0pk.a \
    libsb_cfgP384CPc1pk.a \
    libsb_cfgP384CPc2pk.a \
    libsb_cfgP384CXPc0sw.a \
    libsb_cfgP384CXPc1sw.a \
    libsb_cfgP384CXPc2sw.a \
    libsb_cfgP384CXPKc0sw.a \
    libsb_cfgP384CXPKc1sw.a \
    libsb_cfgP384CXPKc2sw.a \
    libsb_cfgP384CNc0sm.a \
    libsb_cfgP384CNc1sm.a \
    libsb_cfgP384CNc2sm.a \
    libsb_cfgP384CXPc0sm.a \
    libsb_cfgP384CXPc1sm.a \
    libsb_cfgP384CXPc2sm.a \
    libsb_cfgP384CXPKc0sm.a \
    libsb_cfgP384CXPKc1sm.a \
    libsb_cfgP384CXPKc2sm.a \
    libsb_cfgP384CXNc0sm.a \
    libsb_cfgP384CXNc1sm.a \
    libsb_cfgP384CXNc2sm.a \
    libsb_cfgP384CXNKc0sm.a \
    libsb_cfgP384CXNKc1sm.a \
    libsb_cfgP384CXNKc2sm.a \
    libsb_cfgP521IPc0sw.a \
    libsb_cfgP521IPc1sw.a \
    libsb_cfgP521IPc2sw.a \
    libsb_cfgP521CPc0sw.a \
    libsb_cfgP521CPc1sw.a \
    libsb_cfgP521CPc2sw.a \
    libsb_cfgP521IPc0sm.a \
    libsb_cfgP521IPc1sm.a \
    libsb_cfgP521IPc2sm.a \
    libsb_cfgP521CPc0sm.a \
    libsb_cfgP521CPc1sm.a \
    libsb_cfgP521CPc2sm.a \
    libsb_cfgP521IPc0pk.a \
    libsb_cfgP521IPc1pk.a \
    libsb_cfgP521IPc2pk.a \
    libsb_cfgP521CPc0pk.a \
    libsb_cfgP521CPc1pk.a \
    libsb_cfgP521CPc2pk.a \
    libsb_cfgP521CXPc0sw.a \
    libsb_cfgP521CXPc1sw.a \
    libsb_cfgP521CXPc2sw.a \
    libsb_cfgP521CXPKc0sw.a \
    libsb_cfgP521CXPKc1sw.a \
    libsb_cfgP521CXPKc2sw.a \
    libsb_cfgP521CNc0sm.a \
    libsb_cfgP521CNc1sm.a \
    libsb_cfgP521CNc2sm.a \
    libsb_cfgP521CXPc0sm.a \
    libsb_cfgP521CXPc1sm.a \
    libsb_cfgP521CXPc2sm.a \
    libsb_cfgP521CXPKc0sm.a \
    libsb_cfgP521CXPKc1sm.a \
    libsb_cfgP521CXPKc2sm.a \
    libsb_cfgP521CXNc0sm.a \
    libsb_cfgP521CXNc1sm.a \
    libsb_cfgP521CXNc2sm.a \
    libsb_cfgP521CXNKc0sm.a \
    libsb_cfgP521CXNKc1sm.a \
    libsb_cfgP521CXNKc2sm.a

if ENABLE_BARE
LIBSBHW_TARGET = \
    libtarget_bare.a
EXTRA_LIBRARIES += $(LIBSBHW_TARGET)
LIBSBHW_TARGET_CM = $(LIBSBHW_TARGET)
LIBSBHW_TARGET_CMPK = $(LIBSBHW_TARGET)
LIBSBHW_TARGET_PE = $(LIBSBHW_TARGET)
LIBSBHW_TARGET_PEPK = $(LIBSBHW_TARGET)
LIBSBHW_TARGET_PK = $(LIBSBHW_TARGET)
LIBSBHW_TARGET_SM = $(LIBSBHW_TARGET)
endif

if ENABLE_VERSATILE
LIBSBHW_TARGET = \
    libtarget_versatile.a \
    libumdevxs.a
EXTRA_LIBRARIES += $(LIBSBHW_TARGET)
LIBSBHW_TARGET_SM = $(LIBSBHW_TARGET)
LIBSBHW_TARGET_CM = $(LIBSBHW_TARGET)
LIBSBHW_TARGET_CMPK = $(LIBSBHW_TARGET)
LIBSBHW_TARGET_PE = $(LIBSBHW_TARGET)
LIBSBHW_TARGET_PEPK = $(LIBSBHW_TARGET)
LIBSBHW_TARGET_PK = $(LIBSBHW_TARGET)
endif



SB_SW_TEST_LIBRARIES_LIST = libsbsw224.a
SB_SW_TEST_PROGRAMS_LIST = sbswtest224


check_PROGRAMS += $(SB_SW_TEST_PROGRAMS_LIST)
check_LIBRARIES = libsfzutf.a libdebug.a

SBIF_ATTRIBUTE_SUPPORT_SOURCES = \
    $(top_src)/SBCommon/SBIF/src/sbif_attributes.c

ABSTRACTION_INCLUDES = \
    -I$(top_src)/Kit/DriverFramework/$(ABSTRACTION_BASICDEFS)/incl \
    -I$(top_src)/Kit/DriverFramework/$(ABSTRACTION_CLIB)/incl

LIBSB_SW_INCLUDES = \
    $(CONFIGURATION_INCLUDES) \
    -I$(top_src)/SBCommon/Config \
    -I$(top_src)/Framework/PUBDEFS/incl \
    -I$(top_src)/Framework/IMPLDEFS/incl \
    -I$(top_src)/Framework/CLIB/incl \
    -I$(top_src)/SBLib/SB_API/incl \
    -I$(top_src)/SBLib/SB_HYBRID/src \
    -I$(top_src)/SBCommon/SBIF_API/incl \
    -I$(top_src)/SBCommon/SBIF_TOOLS/incl \
    -I$(top_src)/SBCommon/SB_SW_CRYPTO/incl \
    -I$(top_src)/SBCommon/SB_AES_API/incl \
    -I$(top_src)/micro-ecc \
    -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1 -DuECC_WORD_SIZE=4 -DVERIFY_ONLY

LIBSB_SW224_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    $(LIBSB_SW_INCLUDES) \
    -DuECC_SUPPORTS_secp224r1=1

LIBSB_SW224AES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC224_AES256 \
    $(LIBSB_SW_INCLUDES) \
    -DuECC_SUPPORTS_secp224r1=1

LIBSB_SW224KDKAES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC224_KDK_AES256 \
    $(LIBSB_SW_INCLUDES) \
    -DuECC_SUPPORTS_secp224r1=1

LIBSB_SW256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC256 \
    $(LIBSB_SW_INCLUDES) \
    -DuECC_SUPPORTS_secp256r1=1

LIBSB_SW256AES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC256_AES256 \
    $(LIBSB_SW_INCLUDES) \
    -DuECC_SUPPORTS_secp256r1=1

LIBSB_SW256KDKAES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC256_KDK_AES256 \
    $(LIBSB_SW_INCLUDES) \
    -DuECC_SUPPORTS_secp256r1=1

LIBSB_SW384_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC384 \
    $(LIBSB_SW_INCLUDES) \
    -DuECC_SUPPORTS_secp384r1=1

LIBSB_SW384AES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC384_AES256 \
    $(LIBSB_SW_INCLUDES) \
    -DuECC_SUPPORTS_secp384r1=1

LIBSB_SW384KDKAES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC384_KDK_AES256 \
    $(LIBSB_SW_INCLUDES) \
    -DuECC_SUPPORTS_secp384r1=1

LIBSB_SW521_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC521 \
    $(LIBSB_SW_INCLUDES) \
    -DuECC_SUPPORTS_secp521r1=1

LIBSB_SW521AES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC521_AES256 \
    $(LIBSB_SW_INCLUDES) \
    -DuECC_SUPPORTS_secp521r1=1

LIBSB_SW521KDKAES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC521_KDK_AES256 \
    $(LIBSB_SW_INCLUDES) \
    -DuECC_SUPPORTS_secp521r1=1

SECUREBOOTSIM_INCLUDES = \
    $(CONFIGURATION_INCLUDES) \
    -I$(top_src)/SBCommon/Config \
    -I$(top_src)/Framework/PUBDEFS/incl \
    -I$(top_src)/Framework/IMPLDEFS/incl \
    -I$(top_src)/Framework/CLIB/incl \
    -I$(top_src)/SFZUTF/incl \
    -I$(top_src)/SBLib/SB_API/incl \
    -I$(top_src)/SBLib/SB_HYBRID/src \
    -I$(top_src)/SBCommon/SBIF_API/incl \
    -I$(top_src)/SBCommon/SBIF_TOOLS/incl \
    -I$(top_src)/SBCommon/SB_SW_CRYPTO/incl \
    $(ABSTRACTION_INCLUDES) \
    -I$(top_src)/Kit/DriverFramework/v4/Device_API/incl \
    -I$(top_src)/Kit/DriverFramework/v4/DMAResource_API/incl \
    $(DMARES_INCLUDE) \
    -I$(top_src)/Integration/UMDevXS/UserPart/incl

SECUREBOOTSIM224_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    $(SECUREBOOTSIM_INCLUDES)

SECUREBOOTSIM224AES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC224_AES256 \
    $(SECUREBOOTSIM_INCLUDES)

SECUREBOOTSIM224KDKAES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC224_KDK_AES256 \
    $(SECUREBOOTSIM_INCLUDES)

SECUREBOOTSIM256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC256 \
    $(SECUREBOOTSIM_INCLUDES)

SECUREBOOTSIM256AES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC256_AES256 \
    $(SECUREBOOTSIM_INCLUDES)

SECUREBOOTSIM256KDKAES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC256_KDK_AES256 \
    $(SECUREBOOTSIM_INCLUDES)

SECUREBOOTSIM384_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC384 \
    $(SECUREBOOTSIM_INCLUDES)

SECUREBOOTSIM384AES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC384_AES256 \
    $(SECUREBOOTSIM_INCLUDES)

SECUREBOOTSIM384KDKAES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC384_KDK_AES256 \
    $(SECUREBOOTSIM_INCLUDES)

SECUREBOOTSIM521_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC521 \
    $(SECUREBOOTSIM_INCLUDES)

SECUREBOOTSIM521AES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC521_AES256 \
    $(SECUREBOOTSIM_INCLUDES)

SECUREBOOTSIM521KDKAES256_INCLUDES = \
    -I$(top_src)/SBCommon/Config/ECC521_KDK_AES256 \
    $(SECUREBOOTSIM_INCLUDES)

# The SBCommon code use by (variants of) Secure Boot
SB_SW_HASH256_SOURCES = \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/minsha_init.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/minsha_update.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/minsha_core.c

SB_SW_HASH512_SOURCES = \
    $(SB_SW_HASH256_SOURCES) \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_hash_sha512.c

SB_SW_ECDSA224_SOURCES = \
    $(top_src)/micro-ecc/uECC.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_ecdsa224.c

SB_SW_ECDSA256_SOURCES = \
    $(top_src)/micro-ecc/uECC.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_ecdsa256.c

SB_SW_ECDSA384_SOURCES = \
    $(top_src)/micro-ecc/uECC.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_ecdsa384.c

SB_SW_ECDSA521_SOURCES = \
    $(top_src)/micro-ecc/uECC.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_ecdsa521.c

SB_SW_ECDSA_SOURCES = \
    $(top_src)/micro-ecc/uECC.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_ecdsa224.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_ecdsa256.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_ecdsa384.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sb_sw_ecdsa521.c

SB_SW_AES_SOURCES = \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/aes_if.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c

# Additional driver sources for PE (EIP-93) version.
SB_PE_DRIVER_SOURCES = \
    $(top_src)/Kit/EIP93_SL/src/eip93_sl.c \
    $(top_src)/Kit/EIP93_SL/src/eip93_arm.c \
    $(top_src)/Kit/RingHelper/src/ringhelper.c

# Additional driver sources for CM (EIP-123) version.
SB_CM_DRIVER_SOURCES = \
    $(top_src)/Kit/EIP123_CM_Tokens/src/cm_tokens_common.c \
    $(top_src)/Kit/EIP123_CM_Tokens/src/cm_tokens_errdetails.c \
    $(top_src)/Kit/EIP123_SL/src/eip123_sl.c \
    $(top_src)/Kit/EIP123_SL/src/eip123_dma.c

# Additional driver sources for SM (EIP-130) version.
SB_SM_DRIVER_SOURCES = \
    $(top_src)/Kit/EIP130/TokenHelper/src/eip130_token_common.c \
    $(top_src)/Kit/EIP130/MailboxControl/src/eip130.c

# Additional driver sources for PK (EIP-28) version.
SB_PK_DRIVER_SOURCES = $(top_src)/Kit/EIP28_SL/src/eip28.c

# Combinations
SB_PEPK_DRIVER_SOURCES = $(SB_PE_DRIVER_SOURCES) $(SB_PK_DRIVER_SOURCES)
SB_CMPK_DRIVER_SOURCES = $(SB_CM_DRIVER_SOURCES) $(SB_PK_DRIVER_SOURCES)

LIBSB_HYBRID_GEN_INCLUDES = \
    $(ABSTRACTION_INCLUDES) \
    -I$(top_src)/Kit/DriverFramework/v4/Device_API/incl \
    -I$(top_src)/Kit/DriverFramework/v4/DMAResource_API/incl \
    $(DMARES_INCLUDE) \
    -I$(top_src)/Integration/UMDevXS/UserPart/incl \
    -I$(top_src)/Kit/EIP28_SL/incl \
    -I$(top_src)/Kit/EIP93_SL/incl \
    -I$(top_src)/Kit/EIP123_SL/incl \
    -I$(top_src)/Kit/EIP123_CM_Tokens/incl \
    -I$(top_src)/Kit/EIP130/MailboxControl/incl \
    -I$(top_src)/Kit/EIP130/MailboxControl/src \
    -I$(top_src)/Kit/EIP130/TokenHelper/incl \
    -I$(top_src)/Kit/EIP130/TokenHelper/src \
    -I$(top_src)/Kit/RingHelper/incl

if ENABLE_DEBUG
LIBSB_HYBRID_GEN_INCLUDES += \
    -I$(top_src)/Kit/Log/incl \
    -I$(top_src)/Kit/Log/src/safezone
endif

LIBSB_HYBRID224_INCLUDES = \
    $(LIBSB_SW224_INCLUDES) \
    $(LIBSB_HYBRID_GEN_INCLUDES)

LIBSB_HYBRID224AES256_INCLUDES = \
    $(LIBSB_SW224AES256_INCLUDES) \
    $(LIBSB_HYBRID_GEN_INCLUDES)

LIBSB_HYBRID224KDKAES256_INCLUDES = \
    $(LIBSB_SW224KDKAES256_INCLUDES) \
    $(LIBSB_HYBRID_GEN_INCLUDES)

LIBSB_HYBRID256_INCLUDES = \
    $(LIBSB_SW256_INCLUDES) \
    $(LIBSB_HYBRID_GEN_INCLUDES)

LIBSB_HYBRID256AES256_INCLUDES = \
    $(LIBSB_SW256AES256_INCLUDES) \
    $(LIBSB_HYBRID_GEN_INCLUDES)

LIBSB_HYBRID256KDKAES256_INCLUDES = \
    $(LIBSB_SW256KDKAES256_INCLUDES) \
    $(LIBSB_HYBRID_GEN_INCLUDES)

LIBSB_HYBRID384_INCLUDES = \
    $(LIBSB_SW384_INCLUDES) \
    $(LIBSB_HYBRID_GEN_INCLUDES)

LIBSB_HYBRID384AES256_INCLUDES = \
    $(LIBSB_SW384AES256_INCLUDES) \
    $(LIBSB_HYBRID_GEN_INCLUDES)

LIBSB_HYBRID384KDKAES256_INCLUDES = \
    $(LIBSB_SW384KDKAES256_INCLUDES) \
    $(LIBSB_HYBRID_GEN_INCLUDES)

LIBSB_HYBRID521_INCLUDES = \
    $(LIBSB_SW521_INCLUDES) \
    $(LIBSB_HYBRID_GEN_INCLUDES)

LIBSB_HYBRID521AES256_INCLUDES = \
    $(LIBSB_SW521AES256_INCLUDES) \
    $(LIBSB_HYBRID_GEN_INCLUDES)

LIBSB_HYBRID521KDKAES256_INCLUDES = \
    $(LIBSB_SW521KDKAES256_INCLUDES) \
    $(LIBSB_HYBRID_GEN_INCLUDES)

# Rules for libsb_cfgP224IPc0sw.a and securebootsim_cfgP224IPc0sw
libsb_cfgP224IPc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224IPc0sw_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224IPc0sw_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES)
securebootsim_cfgP224IPc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc0sw_LDADD = libsb_cfgP224IPc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc1sw.a and securebootsim_cfgP224IPc1sw
libsb_cfgP224IPc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224IPc1sw_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224IPc1sw_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES)
securebootsim_cfgP224IPc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc1sw_LDADD = libsb_cfgP224IPc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc2sw.a and securebootsim_cfgP224IPc2sw
libsb_cfgP224IPc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224IPc2sw_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224IPc2sw_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES)
securebootsim_cfgP224IPc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc2sw_LDADD = libsb_cfgP224IPc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc0sw.a and securebootsim_cfgP224CPc0sw
libsb_cfgP224CPc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CPc0sw_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc0sw_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc0sw_LDADD = libsb_cfgP224CPc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc1sw.a and securebootsim_cfgP224CPc1sw
libsb_cfgP224CPc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CPc1sw_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc1sw_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc1sw_LDADD = libsb_cfgP224CPc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc2sw.a and securebootsim_cfgP224CPc2sw
libsb_cfgP224CPc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CPc2sw_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc2sw_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc2sw_LDADD = libsb_cfgP224CPc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc0sm.a and securebootsim_cfgP224IPc0sm
libsb_cfgP224IPc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224IPc0sm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224IPc0sm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc0sm_LDADD = libsb_cfgP224IPc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc1sm.a and securebootsim_cfgP224IPc1sm
libsb_cfgP224IPc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224IPc1sm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224IPc1sm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc1sm_LDADD = libsb_cfgP224IPc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc2sm.a and securebootsim_cfgP224IPc2sm
libsb_cfgP224IPc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224IPc2sm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224IPc2sm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc2sm_LDADD = libsb_cfgP224IPc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc0sm.a and securebootsim_cfgP224CPc0sm
libsb_cfgP224CPc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CPc0sm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc0sm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc0sm_LDADD = libsb_cfgP224CPc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc1sm.a and securebootsim_cfgP224CPc1sm
libsb_cfgP224CPc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CPc1sm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc1sm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc1sm_LDADD = libsb_cfgP224CPc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc2sm.a and securebootsim_cfgP224CPc2sm
libsb_cfgP224CPc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CPc2sm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc2sm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc2sm_LDADD = libsb_cfgP224CPc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc0cm.a and securebootsim_cfgP224IPc0cm
libsb_cfgP224IPc0cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224IPc0cm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224IPc0cm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc0cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc0cm_LDADD = libsb_cfgP224IPc0cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc0cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc1cm.a and securebootsim_cfgP224IPc1cm
libsb_cfgP224IPc1cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224IPc1cm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224IPc1cm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc1cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc1cm_LDADD = libsb_cfgP224IPc1cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc1cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc2cm.a and securebootsim_cfgP224IPc2cm
libsb_cfgP224IPc2cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224IPc2cm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224IPc2cm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc2cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc2cm_LDADD = libsb_cfgP224IPc2cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc2cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc0cm.a and securebootsim_cfgP224CPc0cm
libsb_cfgP224CPc0cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CPc0cm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc0cm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc0cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc0cm_LDADD = libsb_cfgP224CPc0cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc0cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc1cm.a and securebootsim_cfgP224CPc1cm
libsb_cfgP224CPc1cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CPc1cm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc1cm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc1cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc1cm_LDADD = libsb_cfgP224CPc1cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc1cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc2cm.a and securebootsim_cfgP224CPc2cm
libsb_cfgP224CPc2cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CPc2cm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc2cm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc2cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc2cm_LDADD = libsb_cfgP224CPc2cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc2cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc0cmpk.a and securebootsim_cfgP224IPc0cmpk
libsb_cfgP224IPc0cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224IPc0cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224IPc0cmpk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc0cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc0cmpk_LDADD = libsb_cfgP224IPc0cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc0cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc1cmpk.a and securebootsim_cfgP224IPc1cmpk
libsb_cfgP224IPc1cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224IPc1cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224IPc1cmpk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc1cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc1cmpk_LDADD = libsb_cfgP224IPc1cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc1cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc2cmpk.a and securebootsim_cfgP224IPc2cmpk
libsb_cfgP224IPc2cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224IPc2cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224IPc2cmpk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc2cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc2cmpk_LDADD = libsb_cfgP224IPc2cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc2cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc0cmpk.a and securebootsim_cfgP224CPc0cmpk
libsb_cfgP224CPc0cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CPc0cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc0cmpk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc0cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc0cmpk_LDADD = libsb_cfgP224CPc0cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc0cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc1cmpk.a and securebootsim_cfgP224CPc1cmpk
libsb_cfgP224CPc1cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CPc1cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc1cmpk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc1cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc1cmpk_LDADD = libsb_cfgP224CPc1cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc1cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc2cmpk.a and securebootsim_cfgP224CPc2cmpk
libsb_cfgP224CPc2cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CPc2cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc2cmpk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc2cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc2cmpk_LDADD = libsb_cfgP224CPc2cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc2cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc0pk.a and securebootsim_cfgP224IPc0pk
libsb_cfgP224IPc0pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH256_SOURCES)
libsb_cfgP224IPc0pk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224IPc0pk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc0pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc0pk_LDADD = libsb_cfgP224IPc0pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc0pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc1pk.a and securebootsim_cfgP224IPc1pk
libsb_cfgP224IPc1pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH256_SOURCES)
libsb_cfgP224IPc1pk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224IPc1pk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc1pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc1pk_LDADD = libsb_cfgP224IPc1pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc1pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc2pk.a and securebootsim_cfgP224IPc2pk
libsb_cfgP224IPc2pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH256_SOURCES)
libsb_cfgP224IPc2pk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224IPc2pk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc2pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc2pk_LDADD = libsb_cfgP224IPc2pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc2pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc0pk.a and securebootsim_cfgP224CPc0pk
libsb_cfgP224CPc0pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api_decrypt.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_aes_if.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES)
libsb_cfgP224CPc0pk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc0pk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc0pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc0pk_LDADD = libsb_cfgP224CPc0pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc0pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc1pk.a and securebootsim_cfgP224CPc1pk
libsb_cfgP224CPc1pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api_decrypt.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_aes_if.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES)
libsb_cfgP224CPc1pk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc1pk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc1pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc1pk_LDADD = libsb_cfgP224CPc1pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc1pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc2pk.a and securebootsim_cfgP224CPc2pk
libsb_cfgP224CPc2pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api_decrypt.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_aes_if.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES)
libsb_cfgP224CPc2pk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc2pk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc2pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc2pk_LDADD = libsb_cfgP224CPc2pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc2pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc0pe.a and securebootsim_cfgP224IPc0pe
libsb_cfgP224IPc0pe_a_SOURCES =  $(SB_PE_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PE/src/sbpe_api.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_hw_init.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_sha224.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224IPc0pe_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224IPc0pe_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc0pe_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc0pe_LDADD = libsb_cfgP224IPc0pe.a $(LIBSBHW_TARGET_PE) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc0pe.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc1pe.a and securebootsim_cfgP224IPc1pe
libsb_cfgP224IPc1pe_a_SOURCES =  $(SB_PE_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PE/src/sbpe_api.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_hw_init.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_sha224.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224IPc1pe_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224IPc1pe_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc1pe_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc1pe_LDADD = libsb_cfgP224IPc1pe.a $(LIBSBHW_TARGET_PE) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc1pe.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc2pe.a and securebootsim_cfgP224IPc2pe
libsb_cfgP224IPc2pe_a_SOURCES =  $(SB_PE_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PE/src/sbpe_api.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_hw_init.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_sha224.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224IPc2pe_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224IPc2pe_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc2pe_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc2pe_LDADD = libsb_cfgP224IPc2pe.a $(LIBSBHW_TARGET_PE) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc2pe.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc0pe.a and securebootsim_cfgP224CPc0pe
libsb_cfgP224CPc0pe_a_SOURCES =  $(SB_PE_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PE/src/sbpe_api_decrypt.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_hw_init.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_aes_if.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_sha224.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CPc0pe_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc0pe_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc0pe_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc0pe_LDADD = libsb_cfgP224CPc0pe.a $(LIBSBHW_TARGET_PE) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc0pe.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc1pe.a and securebootsim_cfgP224CPc1pe
libsb_cfgP224CPc1pe_a_SOURCES =  $(SB_PE_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PE/src/sbpe_api_decrypt.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_hw_init.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_aes_if.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_sha224.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CPc1pe_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc1pe_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc1pe_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc1pe_LDADD = libsb_cfgP224CPc1pe.a $(LIBSBHW_TARGET_PE) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc1pe.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc2pe.a and securebootsim_cfgP224CPc2pe
libsb_cfgP224CPc2pe_a_SOURCES =  $(SB_PE_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PE/src/sbpe_api_decrypt.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_hw_init.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_aes_if.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_sha224.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CPc2pe_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc2pe_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc2pe_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc2pe_LDADD = libsb_cfgP224CPc2pe.a $(LIBSBHW_TARGET_PE) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc2pe.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc0pepk.a and securebootsim_cfgP224IPc0pepk
libsb_cfgP224IPc0pepk_a_SOURCES =  $(SB_PEPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_api.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_hw_init.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_sha224.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224IPc0pepk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224IPc0pepk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc0pepk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc0pepk_LDADD = libsb_cfgP224IPc0pepk.a $(LIBSBHW_TARGET_PEPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc0pepk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc1pepk.a and securebootsim_cfgP224IPc1pepk
libsb_cfgP224IPc1pepk_a_SOURCES =  $(SB_PEPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_api.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_hw_init.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_sha224.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224IPc1pepk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224IPc1pepk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc1pepk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc1pepk_LDADD = libsb_cfgP224IPc1pepk.a $(LIBSBHW_TARGET_PEPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc1pepk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc2pepk.a and securebootsim_cfgP224IPc2pepk
libsb_cfgP224IPc2pepk_a_SOURCES =  $(SB_PEPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_api.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_hw_init.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_sha224.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224IPc2pepk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224IPc2pepk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP224IPc2pepk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc2pepk_LDADD = libsb_cfgP224IPc2pepk.a $(LIBSBHW_TARGET_PEPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224IPc2pepk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc0pepk.a and securebootsim_cfgP224CPc0pepk
libsb_cfgP224CPc0pepk_a_SOURCES =  $(SB_PEPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_api_decrypt.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_hw_init.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_aes_if.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_sha224.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CPc0pepk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc0pepk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc0pepk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc0pepk_LDADD = libsb_cfgP224CPc0pepk.a $(LIBSBHW_TARGET_PEPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc0pepk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc1pepk.a and securebootsim_cfgP224CPc1pepk
libsb_cfgP224CPc1pepk_a_SOURCES =  $(SB_PEPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_api_decrypt.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_hw_init.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_aes_if.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_sha224.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CPc1pepk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc1pepk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc1pepk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc1pepk_LDADD = libsb_cfgP224CPc1pepk.a $(LIBSBHW_TARGET_PEPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc1pepk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc2pepk.a and securebootsim_cfgP224CPc2pepk
libsb_cfgP224CPc2pepk_a_SOURCES =  $(SB_PEPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_api_decrypt.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_hw_init.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_aes_if.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_sha224.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CPc2pepk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CPc2pepk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CPc2pepk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc2pepk_LDADD = libsb_cfgP224CPc2pepk.a $(LIBSBHW_TARGET_PEPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CPc2pepk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPc0sw.a and securebootsim_cfgP224CXPc0sw
libsb_cfgP224CXPc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXPc0sw_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPc0sw_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPc0sw_LDADD = libsb_cfgP224CXPc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPc1sw.a and securebootsim_cfgP224CXPc1sw
libsb_cfgP224CXPc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXPc1sw_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPc1sw_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPc1sw_LDADD = libsb_cfgP224CXPc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPc2sw.a and securebootsim_cfgP224CXPc2sw
libsb_cfgP224CXPc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXPc2sw_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPc2sw_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPc2sw_LDADD = libsb_cfgP224CXPc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPKc0sw.a and securebootsim_cfgP224CXPKc0sw
libsb_cfgP224CXPKc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXPKc0sw_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPKc0sw_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPKc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPKc0sw_LDADD = libsb_cfgP224CXPKc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPKc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPKc1sw.a and securebootsim_cfgP224CXPKc1sw
libsb_cfgP224CXPKc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXPKc1sw_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPKc1sw_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPKc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPKc1sw_LDADD = libsb_cfgP224CXPKc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPKc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPKc2sw.a and securebootsim_cfgP224CXPKc2sw
libsb_cfgP224CXPKc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXPKc2sw_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPKc2sw_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPKc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPKc2sw_LDADD = libsb_cfgP224CXPKc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPKc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CNc0sm.a and securebootsim_cfgP224CNc0sm
libsb_cfgP224CNc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CNc0sm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224CNc0sm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CNc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CNc0sm_LDADD = libsb_cfgP224CNc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CNc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CNc1sm.a and securebootsim_cfgP224CNc1sm
libsb_cfgP224CNc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CNc1sm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224CNc1sm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CNc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CNc1sm_LDADD = libsb_cfgP224CNc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CNc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CNc2sm.a and securebootsim_cfgP224CNc2sm
libsb_cfgP224CNc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CNc2sm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224CNc2sm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CNc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CNc2sm_LDADD = libsb_cfgP224CNc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CNc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPc0sm.a and securebootsim_cfgP224CXPc0sm
libsb_cfgP224CXPc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CXPc0sm_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPc0sm_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPc0sm_LDADD = libsb_cfgP224CXPc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPc1sm.a and securebootsim_cfgP224CXPc1sm
libsb_cfgP224CXPc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CXPc1sm_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPc1sm_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPc1sm_LDADD = libsb_cfgP224CXPc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPc2sm.a and securebootsim_cfgP224CXPc2sm
libsb_cfgP224CXPc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CXPc2sm_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPc2sm_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPc2sm_LDADD = libsb_cfgP224CXPc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPKc0sm.a and securebootsim_cfgP224CXPKc0sm
libsb_cfgP224CXPKc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CXPKc0sm_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPKc0sm_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPKc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPKc0sm_LDADD = libsb_cfgP224CXPKc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPKc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPKc1sm.a and securebootsim_cfgP224CXPKc1sm
libsb_cfgP224CXPKc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CXPKc1sm_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPKc1sm_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPKc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPKc1sm_LDADD = libsb_cfgP224CXPKc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPKc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPKc2sm.a and securebootsim_cfgP224CXPKc2sm
libsb_cfgP224CXPKc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CXPKc2sm_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPKc2sm_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPKc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPKc2sm_LDADD = libsb_cfgP224CXPKc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPKc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNc0sm.a and securebootsim_cfgP224CXNc0sm
libsb_cfgP224CXNc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CXNc0sm_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224CXNc0sm_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNc0sm_LDADD = libsb_cfgP224CXNc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNc1sm.a and securebootsim_cfgP224CXNc1sm
libsb_cfgP224CXNc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CXNc1sm_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224CXNc1sm_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNc1sm_LDADD = libsb_cfgP224CXNc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNc2sm.a and securebootsim_cfgP224CXNc2sm
libsb_cfgP224CXNc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CXNc2sm_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224CXNc2sm_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNc2sm_LDADD = libsb_cfgP224CXNc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNKc0sm.a and securebootsim_cfgP224CXNKc0sm
libsb_cfgP224CXNKc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CXNKc0sm_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224CXNKc0sm_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNKc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNKc0sm_LDADD = libsb_cfgP224CXNKc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNKc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNKc1sm.a and securebootsim_cfgP224CXNKc1sm
libsb_cfgP224CXNKc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CXNKc1sm_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224CXNKc1sm_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNKc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNKc1sm_LDADD = libsb_cfgP224CXNKc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNKc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNKc2sm.a and securebootsim_cfgP224CXNKc2sm
libsb_cfgP224CXNKc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CXNKc2sm_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224CXNKc2sm_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNKc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNKc2sm_LDADD = libsb_cfgP224CXNKc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNKc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CNc0cm.a and securebootsim_cfgP224CNc0cm
libsb_cfgP224CNc0cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CNc0cm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224CNc0cm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CNc0cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CNc0cm_LDADD = libsb_cfgP224CNc0cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CNc0cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CNc1cm.a and securebootsim_cfgP224CNc1cm
libsb_cfgP224CNc1cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CNc1cm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224CNc1cm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CNc1cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CNc1cm_LDADD = libsb_cfgP224CNc1cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CNc1cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CNc2cm.a and securebootsim_cfgP224CNc2cm
libsb_cfgP224CNc2cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CNc2cm_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224CNc2cm_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CNc2cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CNc2cm_LDADD = libsb_cfgP224CNc2cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CNc2cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPc0cm.a and securebootsim_cfgP224CXPc0cm
libsb_cfgP224CXPc0cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXPc0cm_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPc0cm_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPc0cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPc0cm_LDADD = libsb_cfgP224CXPc0cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPc0cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPc1cm.a and securebootsim_cfgP224CXPc1cm
libsb_cfgP224CXPc1cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXPc1cm_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPc1cm_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPc1cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPc1cm_LDADD = libsb_cfgP224CXPc1cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPc1cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPc2cm.a and securebootsim_cfgP224CXPc2cm
libsb_cfgP224CXPc2cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXPc2cm_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPc2cm_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPc2cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPc2cm_LDADD = libsb_cfgP224CXPc2cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPc2cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPKc0cm.a and securebootsim_cfgP224CXPKc0cm
libsb_cfgP224CXPKc0cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXPKc0cm_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPKc0cm_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPKc0cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPKc0cm_LDADD = libsb_cfgP224CXPKc0cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPKc0cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPKc1cm.a and securebootsim_cfgP224CXPKc1cm
libsb_cfgP224CXPKc1cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXPKc1cm_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPKc1cm_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPKc1cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPKc1cm_LDADD = libsb_cfgP224CXPKc1cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPKc1cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPKc2cm.a and securebootsim_cfgP224CXPKc2cm
libsb_cfgP224CXPKc2cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXPKc2cm_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPKc2cm_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPKc2cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPKc2cm_LDADD = libsb_cfgP224CXPKc2cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPKc2cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNc0cm.a and securebootsim_cfgP224CXNc0cm
libsb_cfgP224CXNc0cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXNc0cm_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224CXNc0cm_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNc0cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNc0cm_LDADD = libsb_cfgP224CXNc0cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNc0cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNc1cm.a and securebootsim_cfgP224CXNc1cm
libsb_cfgP224CXNc1cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXNc1cm_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224CXNc1cm_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNc1cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNc1cm_LDADD = libsb_cfgP224CXNc1cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNc1cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNc2cm.a and securebootsim_cfgP224CXNc2cm
libsb_cfgP224CXNc2cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXNc2cm_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224CXNc2cm_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNc2cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNc2cm_LDADD = libsb_cfgP224CXNc2cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNc2cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNKc0cm.a and securebootsim_cfgP224CXNKc0cm
libsb_cfgP224CXNKc0cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXNKc0cm_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224CXNKc0cm_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNKc0cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNKc0cm_LDADD = libsb_cfgP224CXNKc0cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNKc0cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNKc1cm.a and securebootsim_cfgP224CXNKc1cm
libsb_cfgP224CXNKc1cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXNKc1cm_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224CXNKc1cm_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNKc1cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNKc1cm_LDADD = libsb_cfgP224CXNKc1cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNKc1cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNKc2cm.a and securebootsim_cfgP224CXNKc2cm
libsb_cfgP224CXNKc2cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha224.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA224_SOURCES)
libsb_cfgP224CXNKc2cm_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224CXNKc2cm_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNKc2cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNKc2cm_LDADD = libsb_cfgP224CXNKc2cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNKc2cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CNc0cmpk.a and securebootsim_cfgP224CNc0cmpk
libsb_cfgP224CNc0cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CNc0cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224CNc0cmpk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CNc0cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CNc0cmpk_LDADD = libsb_cfgP224CNc0cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CNc0cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CNc1cmpk.a and securebootsim_cfgP224CNc1cmpk
libsb_cfgP224CNc1cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CNc1cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224CNc1cmpk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CNc1cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CNc1cmpk_LDADD = libsb_cfgP224CNc1cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CNc1cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CNc2cmpk.a and securebootsim_cfgP224CNc2cmpk
libsb_cfgP224CNc2cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CNc2cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224CNc2cmpk_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CNc2cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CNc2cmpk_LDADD = libsb_cfgP224CNc2cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CNc2cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPc0cmpk.a and securebootsim_cfgP224CXPc0cmpk
libsb_cfgP224CXPc0cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CXPc0cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPc0cmpk_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPc0cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPc0cmpk_LDADD = libsb_cfgP224CXPc0cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPc0cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPc1cmpk.a and securebootsim_cfgP224CXPc1cmpk
libsb_cfgP224CXPc1cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CXPc1cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPc1cmpk_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPc1cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPc1cmpk_LDADD = libsb_cfgP224CXPc1cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPc1cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPc2cmpk.a and securebootsim_cfgP224CXPc2cmpk
libsb_cfgP224CXPc2cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CXPc2cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPc2cmpk_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPc2cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPc2cmpk_LDADD = libsb_cfgP224CXPc2cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPc2cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPKc0cmpk.a and securebootsim_cfgP224CXPKc0cmpk
libsb_cfgP224CXPKc0cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CXPKc0cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPKc0cmpk_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPKc0cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPKc0cmpk_LDADD = libsb_cfgP224CXPKc0cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPKc0cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPKc1cmpk.a and securebootsim_cfgP224CXPKc1cmpk
libsb_cfgP224CXPKc1cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CXPKc1cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPKc1cmpk_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPKc1cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPKc1cmpk_LDADD = libsb_cfgP224CXPKc1cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPKc1cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXPKc2cmpk.a and securebootsim_cfgP224CXPKc2cmpk
libsb_cfgP224CXPKc2cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP224CXPKc2cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP224CXPKc2cmpk_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXPKc2cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXPKc2cmpk_LDADD = libsb_cfgP224CXPKc2cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXPKc2cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNc0cmpk.a and securebootsim_cfgP224CXNc0cmpk
libsb_cfgP224CXNc0cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CXNc0cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224CXNc0cmpk_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNc0cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNc0cmpk_LDADD = libsb_cfgP224CXNc0cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNc0cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNc1cmpk.a and securebootsim_cfgP224CXNc1cmpk
libsb_cfgP224CXNc1cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CXNc1cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224CXNc1cmpk_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNc1cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNc1cmpk_LDADD = libsb_cfgP224CXNc1cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNc1cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNc2cmpk.a and securebootsim_cfgP224CXNc2cmpk
libsb_cfgP224CXNc2cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CXNc2cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224CXNc2cmpk_CPPFLAGS = $(SECUREBOOTSIM224AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNc2cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNc2cmpk_LDADD = libsb_cfgP224CXNc2cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNc2cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNKc0cmpk.a and securebootsim_cfgP224CXNKc0cmpk
libsb_cfgP224CXNKc0cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CXNKc0cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP224CXNKc0cmpk_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNKc0cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNKc0cmpk_LDADD = libsb_cfgP224CXNKc0cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNKc0cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNKc1cmpk.a and securebootsim_cfgP224CXNKc1cmpk
libsb_cfgP224CXNKc1cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CXNKc1cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP224CXNKc1cmpk_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNKc1cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNKc1cmpk_LDADD = libsb_cfgP224CXNKc1cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNKc1cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CXNKc2cmpk.a and securebootsim_cfgP224CXNKc2cmpk
libsb_cfgP224CXNKc2cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha224.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP224CXNKc2cmpk_a_CPPFLAGS = $(LIBSB_HYBRID224KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP224CXNKc2cmpk_CPPFLAGS = $(SECUREBOOTSIM224KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP224CXNKc2cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CXNKc2cmpk_LDADD = libsb_cfgP224CXNKc2cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO224_A) libsb_cfgP224CXNKc2cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc0sw.a and securebootsim_cfgP256IPc0sw
libsb_cfgP256IPc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256IPc0sw_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256IPc0sw_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES)
securebootsim_cfgP256IPc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc0sw_LDADD = libsb_cfgP256IPc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc1sw.a and securebootsim_cfgP256IPc1sw
libsb_cfgP256IPc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256IPc1sw_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256IPc1sw_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES)
securebootsim_cfgP256IPc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc1sw_LDADD = libsb_cfgP256IPc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc2sw.a and securebootsim_cfgP256IPc2sw
libsb_cfgP256IPc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256IPc2sw_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256IPc2sw_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES)
securebootsim_cfgP256IPc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc2sw_LDADD = libsb_cfgP256IPc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc0sw.a and securebootsim_cfgP256CPc0sw
libsb_cfgP256CPc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CPc0sw_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc0sw_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc0sw_LDADD = libsb_cfgP256CPc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc1sw.a and securebootsim_cfgP256CPc1sw
libsb_cfgP256CPc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CPc1sw_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc1sw_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc1sw_LDADD = libsb_cfgP256CPc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc2sw.a and securebootsim_cfgP256CPc2sw
libsb_cfgP256CPc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CPc2sw_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc2sw_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc2sw_LDADD = libsb_cfgP256CPc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc0sm.a and securebootsim_cfgP256IPc0sm
libsb_cfgP256IPc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256IPc0sm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256IPc0sm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc0sm_LDADD = libsb_cfgP256IPc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc1sm.a and securebootsim_cfgP256IPc1sm
libsb_cfgP256IPc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256IPc1sm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256IPc1sm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc1sm_LDADD = libsb_cfgP256IPc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc2sm.a and securebootsim_cfgP256IPc2sm
libsb_cfgP256IPc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256IPc2sm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256IPc2sm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc2sm_LDADD = libsb_cfgP256IPc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc0sm.a and securebootsim_cfgP256CPc0sm
libsb_cfgP256CPc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CPc0sm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc0sm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc0sm_LDADD = libsb_cfgP256CPc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc1sm.a and securebootsim_cfgP256CPc1sm
libsb_cfgP256CPc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CPc1sm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc1sm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc1sm_LDADD = libsb_cfgP256CPc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc2sm.a and securebootsim_cfgP256CPc2sm
libsb_cfgP256CPc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CPc2sm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc2sm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc2sm_LDADD = libsb_cfgP256CPc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc0cm.a and securebootsim_cfgP256IPc0cm
libsb_cfgP256IPc0cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256IPc0cm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256IPc0cm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc0cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc0cm_LDADD = libsb_cfgP256IPc0cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc0cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc1cm.a and securebootsim_cfgP256IPc1cm
libsb_cfgP256IPc1cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256IPc1cm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256IPc1cm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc1cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc1cm_LDADD = libsb_cfgP256IPc1cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc1cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc2cm.a and securebootsim_cfgP256IPc2cm
libsb_cfgP256IPc2cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256IPc2cm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256IPc2cm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc2cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc2cm_LDADD = libsb_cfgP256IPc2cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc2cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc0cm.a and securebootsim_cfgP256CPc0cm
libsb_cfgP256CPc0cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CPc0cm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc0cm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc0cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc0cm_LDADD = libsb_cfgP256CPc0cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc0cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc1cm.a and securebootsim_cfgP256CPc1cm
libsb_cfgP256CPc1cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CPc1cm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc1cm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc1cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc1cm_LDADD = libsb_cfgP256CPc1cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc1cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc2cm.a and securebootsim_cfgP256CPc2cm
libsb_cfgP256CPc2cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CPc2cm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc2cm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc2cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc2cm_LDADD = libsb_cfgP256CPc2cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc2cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc0cmpk.a and securebootsim_cfgP256IPc0cmpk
libsb_cfgP256IPc0cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256IPc0cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256IPc0cmpk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc0cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc0cmpk_LDADD = libsb_cfgP256IPc0cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc0cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc1cmpk.a and securebootsim_cfgP256IPc1cmpk
libsb_cfgP256IPc1cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256IPc1cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256IPc1cmpk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc1cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc1cmpk_LDADD = libsb_cfgP256IPc1cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc1cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc2cmpk.a and securebootsim_cfgP256IPc2cmpk
libsb_cfgP256IPc2cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256IPc2cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256IPc2cmpk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc2cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc2cmpk_LDADD = libsb_cfgP256IPc2cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc2cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc0cmpk.a and securebootsim_cfgP256CPc0cmpk
libsb_cfgP256CPc0cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CPc0cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc0cmpk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc0cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc0cmpk_LDADD = libsb_cfgP256CPc0cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc0cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc1cmpk.a and securebootsim_cfgP256CPc1cmpk
libsb_cfgP256CPc1cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CPc1cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc1cmpk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc1cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc1cmpk_LDADD = libsb_cfgP256CPc1cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc1cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc2cmpk.a and securebootsim_cfgP256CPc2cmpk
libsb_cfgP256CPc2cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CPc2cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc2cmpk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc2cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc2cmpk_LDADD = libsb_cfgP256CPc2cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc2cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc0pk.a and securebootsim_cfgP256IPc0pk
libsb_cfgP256IPc0pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH256_SOURCES)
libsb_cfgP256IPc0pk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256IPc0pk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc0pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc0pk_LDADD = libsb_cfgP256IPc0pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc0pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc1pk.a and securebootsim_cfgP256IPc1pk
libsb_cfgP256IPc1pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH256_SOURCES)
libsb_cfgP256IPc1pk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256IPc1pk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc1pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc1pk_LDADD = libsb_cfgP256IPc1pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc1pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc2pk.a and securebootsim_cfgP256IPc2pk
libsb_cfgP256IPc2pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH256_SOURCES)
libsb_cfgP256IPc2pk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256IPc2pk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc2pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc2pk_LDADD = libsb_cfgP256IPc2pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc2pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc0pk.a and securebootsim_cfgP256CPc0pk
libsb_cfgP256CPc0pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api_decrypt.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_aes_if.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES)
libsb_cfgP256CPc0pk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc0pk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc0pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc0pk_LDADD = libsb_cfgP256CPc0pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc0pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc1pk.a and securebootsim_cfgP256CPc1pk
libsb_cfgP256CPc1pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api_decrypt.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_aes_if.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES)
libsb_cfgP256CPc1pk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc1pk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc1pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc1pk_LDADD = libsb_cfgP256CPc1pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc1pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc2pk.a and securebootsim_cfgP256CPc2pk
libsb_cfgP256CPc2pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api_decrypt.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_aes_if.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES)
libsb_cfgP256CPc2pk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc2pk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc2pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc2pk_LDADD = libsb_cfgP256CPc2pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc2pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc0pe.a and securebootsim_cfgP256IPc0pe
libsb_cfgP256IPc0pe_a_SOURCES =  $(SB_PE_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PE/src/sbpe_api.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_hw_init.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_sha256.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256IPc0pe_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256IPc0pe_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc0pe_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc0pe_LDADD = libsb_cfgP256IPc0pe.a $(LIBSBHW_TARGET_PE) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc0pe.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc1pe.a and securebootsim_cfgP256IPc1pe
libsb_cfgP256IPc1pe_a_SOURCES =  $(SB_PE_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PE/src/sbpe_api.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_hw_init.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_sha256.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256IPc1pe_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256IPc1pe_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc1pe_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc1pe_LDADD = libsb_cfgP256IPc1pe.a $(LIBSBHW_TARGET_PE) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc1pe.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc2pe.a and securebootsim_cfgP256IPc2pe
libsb_cfgP256IPc2pe_a_SOURCES =  $(SB_PE_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PE/src/sbpe_api.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_hw_init.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_sha256.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256IPc2pe_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256IPc2pe_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc2pe_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc2pe_LDADD = libsb_cfgP256IPc2pe.a $(LIBSBHW_TARGET_PE) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc2pe.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc0pe.a and securebootsim_cfgP256CPc0pe
libsb_cfgP256CPc0pe_a_SOURCES =  $(SB_PE_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PE/src/sbpe_api_decrypt.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_hw_init.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_aes_if.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_sha256.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CPc0pe_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc0pe_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc0pe_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc0pe_LDADD = libsb_cfgP256CPc0pe.a $(LIBSBHW_TARGET_PE) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc0pe.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc1pe.a and securebootsim_cfgP256CPc1pe
libsb_cfgP256CPc1pe_a_SOURCES =  $(SB_PE_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PE/src/sbpe_api_decrypt.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_hw_init.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_aes_if.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_sha256.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CPc1pe_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc1pe_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc1pe_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc1pe_LDADD = libsb_cfgP256CPc1pe.a $(LIBSBHW_TARGET_PE) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc1pe.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc2pe.a and securebootsim_cfgP256CPc2pe
libsb_cfgP256CPc2pe_a_SOURCES =  $(SB_PE_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PE/src/sbpe_api_decrypt.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_hw_init.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_aes_if.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_eip93_sha256.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PE/src/sbpe_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CPc2pe_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc2pe_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc2pe_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc2pe_LDADD = libsb_cfgP256CPc2pe.a $(LIBSBHW_TARGET_PE) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc2pe.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc0pepk.a and securebootsim_cfgP256IPc0pepk
libsb_cfgP256IPc0pepk_a_SOURCES =  $(SB_PEPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_api.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_hw_init.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_sha256.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256IPc0pepk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256IPc0pepk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc0pepk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc0pepk_LDADD = libsb_cfgP256IPc0pepk.a $(LIBSBHW_TARGET_PEPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc0pepk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc1pepk.a and securebootsim_cfgP256IPc1pepk
libsb_cfgP256IPc1pepk_a_SOURCES =  $(SB_PEPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_api.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_hw_init.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_sha256.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256IPc1pepk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256IPc1pepk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc1pepk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc1pepk_LDADD = libsb_cfgP256IPc1pepk.a $(LIBSBHW_TARGET_PEPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc1pepk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc2pepk.a and securebootsim_cfgP256IPc2pepk
libsb_cfgP256IPc2pepk_a_SOURCES =  $(SB_PEPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_api.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_hw_init.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_sha256.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256IPc2pepk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256IPc2pepk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP256IPc2pepk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc2pepk_LDADD = libsb_cfgP256IPc2pepk.a $(LIBSBHW_TARGET_PEPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256IPc2pepk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc0pepk.a and securebootsim_cfgP256CPc0pepk
libsb_cfgP256CPc0pepk_a_SOURCES =  $(SB_PEPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_api_decrypt.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_hw_init.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_aes_if.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_sha256.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CPc0pepk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc0pepk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc0pepk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc0pepk_LDADD = libsb_cfgP256CPc0pepk.a $(LIBSBHW_TARGET_PEPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc0pepk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc1pepk.a and securebootsim_cfgP256CPc1pepk
libsb_cfgP256CPc1pepk_a_SOURCES =  $(SB_PEPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_api_decrypt.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_hw_init.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_aes_if.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_sha256.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CPc1pepk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc1pepk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc1pepk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc1pepk_LDADD = libsb_cfgP256CPc1pepk.a $(LIBSBHW_TARGET_PEPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc1pepk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc2pepk.a and securebootsim_cfgP256CPc2pepk
libsb_cfgP256CPc2pepk_a_SOURCES =  $(SB_PEPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_api_decrypt.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_hw_init.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_aes_if.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_eip93_sha256.c \
    $(top_src)/SBLib/SB_PEPK/src/sbpepk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CPc2pepk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CPc2pepk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CPc2pepk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc2pepk_LDADD = libsb_cfgP256CPc2pepk.a $(LIBSBHW_TARGET_PEPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CPc2pepk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPc0sw.a and securebootsim_cfgP256CXPc0sw
libsb_cfgP256CXPc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXPc0sw_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPc0sw_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPc0sw_LDADD = libsb_cfgP256CXPc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPc1sw.a and securebootsim_cfgP256CXPc1sw
libsb_cfgP256CXPc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXPc1sw_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPc1sw_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPc1sw_LDADD = libsb_cfgP256CXPc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPc2sw.a and securebootsim_cfgP256CXPc2sw
libsb_cfgP256CXPc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXPc2sw_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPc2sw_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPc2sw_LDADD = libsb_cfgP256CXPc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPKc0sw.a and securebootsim_cfgP256CXPKc0sw
libsb_cfgP256CXPKc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXPKc0sw_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPKc0sw_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPKc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPKc0sw_LDADD = libsb_cfgP256CXPKc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPKc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPKc1sw.a and securebootsim_cfgP256CXPKc1sw
libsb_cfgP256CXPKc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXPKc1sw_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPKc1sw_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPKc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPKc1sw_LDADD = libsb_cfgP256CXPKc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPKc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPKc2sw.a and securebootsim_cfgP256CXPKc2sw
libsb_cfgP256CXPKc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXPKc2sw_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPKc2sw_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPKc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPKc2sw_LDADD = libsb_cfgP256CXPKc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPKc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CNc0sm.a and securebootsim_cfgP256CNc0sm
libsb_cfgP256CNc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CNc0sm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256CNc0sm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CNc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CNc0sm_LDADD = libsb_cfgP256CNc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CNc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CNc1sm.a and securebootsim_cfgP256CNc1sm
libsb_cfgP256CNc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CNc1sm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256CNc1sm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CNc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CNc1sm_LDADD = libsb_cfgP256CNc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CNc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CNc2sm.a and securebootsim_cfgP256CNc2sm
libsb_cfgP256CNc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CNc2sm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256CNc2sm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CNc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CNc2sm_LDADD = libsb_cfgP256CNc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CNc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPc0sm.a and securebootsim_cfgP256CXPc0sm
libsb_cfgP256CXPc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CXPc0sm_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPc0sm_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPc0sm_LDADD = libsb_cfgP256CXPc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPc1sm.a and securebootsim_cfgP256CXPc1sm
libsb_cfgP256CXPc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CXPc1sm_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPc1sm_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPc1sm_LDADD = libsb_cfgP256CXPc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPc2sm.a and securebootsim_cfgP256CXPc2sm
libsb_cfgP256CXPc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CXPc2sm_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPc2sm_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPc2sm_LDADD = libsb_cfgP256CXPc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPKc0sm.a and securebootsim_cfgP256CXPKc0sm
libsb_cfgP256CXPKc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CXPKc0sm_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPKc0sm_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPKc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPKc0sm_LDADD = libsb_cfgP256CXPKc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPKc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPKc1sm.a and securebootsim_cfgP256CXPKc1sm
libsb_cfgP256CXPKc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CXPKc1sm_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPKc1sm_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPKc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPKc1sm_LDADD = libsb_cfgP256CXPKc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPKc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPKc2sm.a and securebootsim_cfgP256CXPKc2sm
libsb_cfgP256CXPKc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CXPKc2sm_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPKc2sm_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPKc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPKc2sm_LDADD = libsb_cfgP256CXPKc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPKc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNc0sm.a and securebootsim_cfgP256CXNc0sm
libsb_cfgP256CXNc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CXNc0sm_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256CXNc0sm_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNc0sm_LDADD = libsb_cfgP256CXNc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNc1sm.a and securebootsim_cfgP256CXNc1sm
libsb_cfgP256CXNc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CXNc1sm_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256CXNc1sm_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNc1sm_LDADD = libsb_cfgP256CXNc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNc2sm.a and securebootsim_cfgP256CXNc2sm
libsb_cfgP256CXNc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CXNc2sm_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256CXNc2sm_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNc2sm_LDADD = libsb_cfgP256CXNc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNKc0sm.a and securebootsim_cfgP256CXNKc0sm
libsb_cfgP256CXNKc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CXNKc0sm_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256CXNKc0sm_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNKc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNKc0sm_LDADD = libsb_cfgP256CXNKc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNKc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNKc1sm.a and securebootsim_cfgP256CXNKc1sm
libsb_cfgP256CXNKc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CXNKc1sm_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256CXNKc1sm_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNKc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNKc1sm_LDADD = libsb_cfgP256CXNKc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNKc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNKc2sm.a and securebootsim_cfgP256CXNKc2sm
libsb_cfgP256CXNKc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CXNKc2sm_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256CXNKc2sm_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNKc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNKc2sm_LDADD = libsb_cfgP256CXNKc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNKc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CNc0cm.a and securebootsim_cfgP256CNc0cm
libsb_cfgP256CNc0cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CNc0cm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256CNc0cm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CNc0cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CNc0cm_LDADD = libsb_cfgP256CNc0cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CNc0cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CNc1cm.a and securebootsim_cfgP256CNc1cm
libsb_cfgP256CNc1cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CNc1cm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256CNc1cm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CNc1cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CNc1cm_LDADD = libsb_cfgP256CNc1cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CNc1cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CNc2cm.a and securebootsim_cfgP256CNc2cm
libsb_cfgP256CNc2cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CNc2cm_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256CNc2cm_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CNc2cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CNc2cm_LDADD = libsb_cfgP256CNc2cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CNc2cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPc0cm.a and securebootsim_cfgP256CXPc0cm
libsb_cfgP256CXPc0cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXPc0cm_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPc0cm_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPc0cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPc0cm_LDADD = libsb_cfgP256CXPc0cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPc0cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPc1cm.a and securebootsim_cfgP256CXPc1cm
libsb_cfgP256CXPc1cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXPc1cm_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPc1cm_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPc1cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPc1cm_LDADD = libsb_cfgP256CXPc1cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPc1cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPc2cm.a and securebootsim_cfgP256CXPc2cm
libsb_cfgP256CXPc2cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXPc2cm_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPc2cm_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPc2cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPc2cm_LDADD = libsb_cfgP256CXPc2cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPc2cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPKc0cm.a and securebootsim_cfgP256CXPKc0cm
libsb_cfgP256CXPKc0cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXPKc0cm_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPKc0cm_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPKc0cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPKc0cm_LDADD = libsb_cfgP256CXPKc0cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPKc0cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPKc1cm.a and securebootsim_cfgP256CXPKc1cm
libsb_cfgP256CXPKc1cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXPKc1cm_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPKc1cm_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPKc1cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPKc1cm_LDADD = libsb_cfgP256CXPKc1cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPKc1cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPKc2cm.a and securebootsim_cfgP256CXPKc2cm
libsb_cfgP256CXPKc2cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXPKc2cm_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPKc2cm_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPKc2cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPKc2cm_LDADD = libsb_cfgP256CXPKc2cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPKc2cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNc0cm.a and securebootsim_cfgP256CXNc0cm
libsb_cfgP256CXNc0cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXNc0cm_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256CXNc0cm_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNc0cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNc0cm_LDADD = libsb_cfgP256CXNc0cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNc0cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNc1cm.a and securebootsim_cfgP256CXNc1cm
libsb_cfgP256CXNc1cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXNc1cm_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256CXNc1cm_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNc1cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNc1cm_LDADD = libsb_cfgP256CXNc1cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNc1cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNc2cm.a and securebootsim_cfgP256CXNc2cm
libsb_cfgP256CXNc2cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXNc2cm_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256CXNc2cm_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNc2cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNc2cm_LDADD = libsb_cfgP256CXNc2cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNc2cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNKc0cm.a and securebootsim_cfgP256CXNKc0cm
libsb_cfgP256CXNKc0cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXNKc0cm_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256CXNKc0cm_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNKc0cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNKc0cm_LDADD = libsb_cfgP256CXNKc0cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNKc0cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNKc1cm.a and securebootsim_cfgP256CXNKc1cm
libsb_cfgP256CXNKc1cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXNKc1cm_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256CXNKc1cm_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNKc1cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNKc1cm_LDADD = libsb_cfgP256CXNKc1cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNKc1cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNKc2cm.a and securebootsim_cfgP256CXNKc2cm
libsb_cfgP256CXNKc2cm_a_SOURCES =  $(SB_CM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CM/src/sbcm_api_decrypt.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_hw_init.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_system_info.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_eip123_sha256.c \
    $(top_src)/SBLib/SB_CM/src/sbcm_sw_ecdsa_verify.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_ECDSA256_SOURCES)
libsb_cfgP256CXNKc2cm_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256CXNKc2cm_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNKc2cm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNKc2cm_LDADD = libsb_cfgP256CXNKc2cm.a $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNKc2cm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CNc0cmpk.a and securebootsim_cfgP256CNc0cmpk
libsb_cfgP256CNc0cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CNc0cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256CNc0cmpk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CNc0cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CNc0cmpk_LDADD = libsb_cfgP256CNc0cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CNc0cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CNc1cmpk.a and securebootsim_cfgP256CNc1cmpk
libsb_cfgP256CNc1cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CNc1cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256CNc1cmpk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CNc1cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CNc1cmpk_LDADD = libsb_cfgP256CNc1cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CNc1cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CNc2cmpk.a and securebootsim_cfgP256CNc2cmpk
libsb_cfgP256CNc2cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CNc2cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256CNc2cmpk_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CNc2cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CNc2cmpk_LDADD = libsb_cfgP256CNc2cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CNc2cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPc0cmpk.a and securebootsim_cfgP256CXPc0cmpk
libsb_cfgP256CXPc0cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CXPc0cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPc0cmpk_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPc0cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPc0cmpk_LDADD = libsb_cfgP256CXPc0cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPc0cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPc1cmpk.a and securebootsim_cfgP256CXPc1cmpk
libsb_cfgP256CXPc1cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CXPc1cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPc1cmpk_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPc1cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPc1cmpk_LDADD = libsb_cfgP256CXPc1cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPc1cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPc2cmpk.a and securebootsim_cfgP256CXPc2cmpk
libsb_cfgP256CXPc2cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CXPc2cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPc2cmpk_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPc2cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPc2cmpk_LDADD = libsb_cfgP256CXPc2cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPc2cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPKc0cmpk.a and securebootsim_cfgP256CXPKc0cmpk
libsb_cfgP256CXPKc0cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CXPKc0cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPKc0cmpk_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPKc0cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPKc0cmpk_LDADD = libsb_cfgP256CXPKc0cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPKc0cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPKc1cmpk.a and securebootsim_cfgP256CXPKc1cmpk
libsb_cfgP256CXPKc1cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CXPKc1cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPKc1cmpk_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPKc1cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPKc1cmpk_LDADD = libsb_cfgP256CXPKc1cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPKc1cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXPKc2cmpk.a and securebootsim_cfgP256CXPKc2cmpk
libsb_cfgP256CXPKc2cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP256CXPKc2cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP256CXPKc2cmpk_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXPKc2cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXPKc2cmpk_LDADD = libsb_cfgP256CXPKc2cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXPKc2cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNc0cmpk.a and securebootsim_cfgP256CXNc0cmpk
libsb_cfgP256CXNc0cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CXNc0cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256CXNc0cmpk_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNc0cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNc0cmpk_LDADD = libsb_cfgP256CXNc0cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNc0cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNc1cmpk.a and securebootsim_cfgP256CXNc1cmpk
libsb_cfgP256CXNc1cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CXNc1cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256CXNc1cmpk_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNc1cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNc1cmpk_LDADD = libsb_cfgP256CXNc1cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNc1cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNc2cmpk.a and securebootsim_cfgP256CXNc2cmpk
libsb_cfgP256CXNc2cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CXNc2cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256CXNc2cmpk_CPPFLAGS = $(SECUREBOOTSIM256AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNc2cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNc2cmpk_LDADD = libsb_cfgP256CXNc2cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNc2cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNKc0cmpk.a and securebootsim_cfgP256CXNKc0cmpk
libsb_cfgP256CXNKc0cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CXNKc0cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP256CXNKc0cmpk_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNKc0cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNKc0cmpk_LDADD = libsb_cfgP256CXNKc0cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNKc0cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNKc1cmpk.a and securebootsim_cfgP256CXNKc1cmpk
libsb_cfgP256CXNKc1cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CXNKc1cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP256CXNKc1cmpk_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNKc1cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNKc1cmpk_LDADD = libsb_cfgP256CXNKc1cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNKc1cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CXNKc2cmpk.a and securebootsim_cfgP256CXNKc2cmpk
libsb_cfgP256CXNKc2cmpk_a_SOURCES =  $(SB_CMPK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_api_decrypt.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_hw_init.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_aes_if.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_publicdata.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_system_info.c \
    $(top_src)/SBLib/SB_CMPK/src/sbcmpk_eip123_sha256.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP256CXNKc2cmpk_a_CPPFLAGS = $(LIBSB_HYBRID256KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP256CXNKc2cmpk_CPPFLAGS = $(SECUREBOOTSIM256KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP256CXNKc2cmpk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CXNKc2cmpk_LDADD = libsb_cfgP256CXNKc2cmpk.a $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO256_A) libsb_cfgP256CXNKc2cmpk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384IPc0sw.a and securebootsim_cfgP384IPc0sw
libsb_cfgP384IPc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA384_SOURCES)
libsb_cfgP384IPc0sw_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP384IPc0sw_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES)
securebootsim_cfgP384IPc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP384IPc0sw_LDADD = libsb_cfgP384IPc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO384_A) libsb_cfgP384IPc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384IPc1sw.a and securebootsim_cfgP384IPc1sw
libsb_cfgP384IPc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA384_SOURCES)
libsb_cfgP384IPc1sw_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP384IPc1sw_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES)
securebootsim_cfgP384IPc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP384IPc1sw_LDADD = libsb_cfgP384IPc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO384_A) libsb_cfgP384IPc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384IPc2sw.a and securebootsim_cfgP384IPc2sw
libsb_cfgP384IPc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA384_SOURCES)
libsb_cfgP384IPc2sw_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP384IPc2sw_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES)
securebootsim_cfgP384IPc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP384IPc2sw_LDADD = libsb_cfgP384IPc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO384_A) libsb_cfgP384IPc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CPc0sw.a and securebootsim_cfgP384CPc0sw
libsb_cfgP384CPc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA384_SOURCES)
libsb_cfgP384CPc0sw_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CPc0sw_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CPc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CPc0sw_LDADD = libsb_cfgP384CPc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO384_A) libsb_cfgP384CPc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CPc1sw.a and securebootsim_cfgP384CPc1sw
libsb_cfgP384CPc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA384_SOURCES)
libsb_cfgP384CPc1sw_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CPc1sw_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CPc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CPc1sw_LDADD = libsb_cfgP384CPc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO384_A) libsb_cfgP384CPc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CPc2sw.a and securebootsim_cfgP384CPc2sw
libsb_cfgP384CPc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA384_SOURCES)
libsb_cfgP384CPc2sw_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CPc2sw_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CPc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CPc2sw_LDADD = libsb_cfgP384CPc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO384_A) libsb_cfgP384CPc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384IPc0sm.a and securebootsim_cfgP384IPc0sm
libsb_cfgP384IPc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP384IPc0sm_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP384IPc0sm_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP384IPc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP384IPc0sm_LDADD = libsb_cfgP384IPc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384IPc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384IPc1sm.a and securebootsim_cfgP384IPc1sm
libsb_cfgP384IPc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP384IPc1sm_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP384IPc1sm_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP384IPc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP384IPc1sm_LDADD = libsb_cfgP384IPc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384IPc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384IPc2sm.a and securebootsim_cfgP384IPc2sm
libsb_cfgP384IPc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP384IPc2sm_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP384IPc2sm_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP384IPc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP384IPc2sm_LDADD = libsb_cfgP384IPc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384IPc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CPc0sm.a and securebootsim_cfgP384CPc0sm
libsb_cfgP384CPc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP384CPc0sm_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CPc0sm_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CPc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CPc0sm_LDADD = libsb_cfgP384CPc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CPc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CPc1sm.a and securebootsim_cfgP384CPc1sm
libsb_cfgP384CPc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP384CPc1sm_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CPc1sm_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CPc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CPc1sm_LDADD = libsb_cfgP384CPc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CPc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CPc2sm.a and securebootsim_cfgP384CPc2sm
libsb_cfgP384CPc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP384CPc2sm_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CPc2sm_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CPc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CPc2sm_LDADD = libsb_cfgP384CPc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CPc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384IPc0pk.a and securebootsim_cfgP384IPc0pk
libsb_cfgP384IPc0pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH512_SOURCES)
libsb_cfgP384IPc0pk_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP384IPc0pk_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP384IPc0pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP384IPc0pk_LDADD = libsb_cfgP384IPc0pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384IPc0pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384IPc1pk.a and securebootsim_cfgP384IPc1pk
libsb_cfgP384IPc1pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH512_SOURCES)
libsb_cfgP384IPc1pk_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP384IPc1pk_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP384IPc1pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP384IPc1pk_LDADD = libsb_cfgP384IPc1pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384IPc1pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384IPc2pk.a and securebootsim_cfgP384IPc2pk
libsb_cfgP384IPc2pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH512_SOURCES)
libsb_cfgP384IPc2pk_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP384IPc2pk_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP384IPc2pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP384IPc2pk_LDADD = libsb_cfgP384IPc2pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384IPc2pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CPc0pk.a and securebootsim_cfgP384CPc0pk
libsb_cfgP384CPc0pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api_decrypt.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_aes_if.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES)
libsb_cfgP384CPc0pk_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CPc0pk_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CPc0pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CPc0pk_LDADD = libsb_cfgP384CPc0pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CPc0pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CPc1pk.a and securebootsim_cfgP384CPc1pk
libsb_cfgP384CPc1pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api_decrypt.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_aes_if.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES)
libsb_cfgP384CPc1pk_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CPc1pk_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CPc1pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CPc1pk_LDADD = libsb_cfgP384CPc1pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CPc1pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CPc2pk.a and securebootsim_cfgP384CPc2pk
libsb_cfgP384CPc2pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api_decrypt.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_aes_if.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES)
libsb_cfgP384CPc2pk_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CPc2pk_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CPc2pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CPc2pk_LDADD = libsb_cfgP384CPc2pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CPc2pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXPc0sw.a and securebootsim_cfgP384CXPc0sw
libsb_cfgP384CXPc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA384_SOURCES)
libsb_cfgP384CXPc0sw_a_CPPFLAGS = $(LIBSB_HYBRID384AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CXPc0sw_CPPFLAGS = $(SECUREBOOTSIM384AES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXPc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXPc0sw_LDADD = libsb_cfgP384CXPc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXPc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXPc1sw.a and securebootsim_cfgP384CXPc1sw
libsb_cfgP384CXPc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA384_SOURCES)
libsb_cfgP384CXPc1sw_a_CPPFLAGS = $(LIBSB_HYBRID384AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CXPc1sw_CPPFLAGS = $(SECUREBOOTSIM384AES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXPc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXPc1sw_LDADD = libsb_cfgP384CXPc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXPc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXPc2sw.a and securebootsim_cfgP384CXPc2sw
libsb_cfgP384CXPc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA384_SOURCES)
libsb_cfgP384CXPc2sw_a_CPPFLAGS = $(LIBSB_HYBRID384AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CXPc2sw_CPPFLAGS = $(SECUREBOOTSIM384AES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXPc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXPc2sw_LDADD = libsb_cfgP384CXPc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXPc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXPKc0sw.a and securebootsim_cfgP384CXPKc0sw
libsb_cfgP384CXPKc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA384_SOURCES)
libsb_cfgP384CXPKc0sw_a_CPPFLAGS = $(LIBSB_HYBRID384KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CXPKc0sw_CPPFLAGS = $(SECUREBOOTSIM384KDKAES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXPKc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXPKc0sw_LDADD = libsb_cfgP384CXPKc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXPKc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXPKc1sw.a and securebootsim_cfgP384CXPKc1sw
libsb_cfgP384CXPKc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA384_SOURCES)
libsb_cfgP384CXPKc1sw_a_CPPFLAGS = $(LIBSB_HYBRID384KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CXPKc1sw_CPPFLAGS = $(SECUREBOOTSIM384KDKAES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXPKc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXPKc1sw_LDADD = libsb_cfgP384CXPKc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXPKc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXPKc2sw.a and securebootsim_cfgP384CXPKc2sw
libsb_cfgP384CXPKc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA384_SOURCES)
libsb_cfgP384CXPKc2sw_a_CPPFLAGS = $(LIBSB_HYBRID384KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CXPKc2sw_CPPFLAGS = $(SECUREBOOTSIM384KDKAES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXPKc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXPKc2sw_LDADD = libsb_cfgP384CXPKc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXPKc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CNc0sm.a and securebootsim_cfgP384CNc0sm
libsb_cfgP384CNc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP384CNc0sm_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP384CNc0sm_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CNc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CNc0sm_LDADD = libsb_cfgP384CNc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CNc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CNc1sm.a and securebootsim_cfgP384CNc1sm
libsb_cfgP384CNc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP384CNc1sm_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP384CNc1sm_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CNc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CNc1sm_LDADD = libsb_cfgP384CNc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CNc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CNc2sm.a and securebootsim_cfgP384CNc2sm
libsb_cfgP384CNc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP384CNc2sm_a_CPPFLAGS = $(LIBSB_HYBRID384_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP384CNc2sm_CPPFLAGS = $(SECUREBOOTSIM384_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CNc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CNc2sm_LDADD = libsb_cfgP384CNc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CNc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXPc0sm.a and securebootsim_cfgP384CXPc0sm
libsb_cfgP384CXPc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP384CXPc0sm_a_CPPFLAGS = $(LIBSB_HYBRID384AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CXPc0sm_CPPFLAGS = $(SECUREBOOTSIM384AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXPc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXPc0sm_LDADD = libsb_cfgP384CXPc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXPc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXPc1sm.a and securebootsim_cfgP384CXPc1sm
libsb_cfgP384CXPc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP384CXPc1sm_a_CPPFLAGS = $(LIBSB_HYBRID384AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CXPc1sm_CPPFLAGS = $(SECUREBOOTSIM384AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXPc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXPc1sm_LDADD = libsb_cfgP384CXPc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXPc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXPc2sm.a and securebootsim_cfgP384CXPc2sm
libsb_cfgP384CXPc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP384CXPc2sm_a_CPPFLAGS = $(LIBSB_HYBRID384AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CXPc2sm_CPPFLAGS = $(SECUREBOOTSIM384AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXPc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXPc2sm_LDADD = libsb_cfgP384CXPc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXPc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXPKc0sm.a and securebootsim_cfgP384CXPKc0sm
libsb_cfgP384CXPKc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP384CXPKc0sm_a_CPPFLAGS = $(LIBSB_HYBRID384KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CXPKc0sm_CPPFLAGS = $(SECUREBOOTSIM384KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXPKc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXPKc0sm_LDADD = libsb_cfgP384CXPKc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXPKc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXPKc1sm.a and securebootsim_cfgP384CXPKc1sm
libsb_cfgP384CXPKc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP384CXPKc1sm_a_CPPFLAGS = $(LIBSB_HYBRID384KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CXPKc1sm_CPPFLAGS = $(SECUREBOOTSIM384KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXPKc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXPKc1sm_LDADD = libsb_cfgP384CXPKc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXPKc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXPKc2sm.a and securebootsim_cfgP384CXPKc2sm
libsb_cfgP384CXPKc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP384CXPKc2sm_a_CPPFLAGS = $(LIBSB_HYBRID384KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP384CXPKc2sm_CPPFLAGS = $(SECUREBOOTSIM384KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXPKc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXPKc2sm_LDADD = libsb_cfgP384CXPKc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXPKc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXNc0sm.a and securebootsim_cfgP384CXNc0sm
libsb_cfgP384CXNc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP384CXNc0sm_a_CPPFLAGS = $(LIBSB_HYBRID384AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP384CXNc0sm_CPPFLAGS = $(SECUREBOOTSIM384AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXNc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXNc0sm_LDADD = libsb_cfgP384CXNc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXNc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXNc1sm.a and securebootsim_cfgP384CXNc1sm
libsb_cfgP384CXNc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP384CXNc1sm_a_CPPFLAGS = $(LIBSB_HYBRID384AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP384CXNc1sm_CPPFLAGS = $(SECUREBOOTSIM384AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXNc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXNc1sm_LDADD = libsb_cfgP384CXNc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXNc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXNc2sm.a and securebootsim_cfgP384CXNc2sm
libsb_cfgP384CXNc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP384CXNc2sm_a_CPPFLAGS = $(LIBSB_HYBRID384AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP384CXNc2sm_CPPFLAGS = $(SECUREBOOTSIM384AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXNc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXNc2sm_LDADD = libsb_cfgP384CXNc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXNc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXNKc0sm.a and securebootsim_cfgP384CXNKc0sm
libsb_cfgP384CXNKc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP384CXNKc0sm_a_CPPFLAGS = $(LIBSB_HYBRID384KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP384CXNKc0sm_CPPFLAGS = $(SECUREBOOTSIM384KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXNKc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXNKc0sm_LDADD = libsb_cfgP384CXNKc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXNKc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXNKc1sm.a and securebootsim_cfgP384CXNKc1sm
libsb_cfgP384CXNKc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP384CXNKc1sm_a_CPPFLAGS = $(LIBSB_HYBRID384KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP384CXNKc1sm_CPPFLAGS = $(SECUREBOOTSIM384KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXNKc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXNKc1sm_LDADD = libsb_cfgP384CXNKc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXNKc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP384CXNKc2sm.a and securebootsim_cfgP384CXNKc2sm
libsb_cfgP384CXNKc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP384CXNKc2sm_a_CPPFLAGS = $(LIBSB_HYBRID384KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP384CXNKc2sm_CPPFLAGS = $(SECUREBOOTSIM384KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP384CXNKc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP384CXNKc2sm_LDADD = libsb_cfgP384CXNKc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO384_A) libsb_cfgP384CXNKc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521IPc0sw.a and securebootsim_cfgP521IPc0sw
libsb_cfgP521IPc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA521_SOURCES)
libsb_cfgP521IPc0sw_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP521IPc0sw_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES)
securebootsim_cfgP521IPc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP521IPc0sw_LDADD = libsb_cfgP521IPc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO521_A) libsb_cfgP521IPc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521IPc1sw.a and securebootsim_cfgP521IPc1sw
libsb_cfgP521IPc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA521_SOURCES)
libsb_cfgP521IPc1sw_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP521IPc1sw_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES)
securebootsim_cfgP521IPc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP521IPc1sw_LDADD = libsb_cfgP521IPc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO521_A) libsb_cfgP521IPc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521IPc2sw.a and securebootsim_cfgP521IPc2sw
libsb_cfgP521IPc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA521_SOURCES)
libsb_cfgP521IPc2sw_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP521IPc2sw_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES)
securebootsim_cfgP521IPc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP521IPc2sw_LDADD = libsb_cfgP521IPc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO521_A) libsb_cfgP521IPc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CPc0sw.a and securebootsim_cfgP521CPc0sw
libsb_cfgP521CPc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA521_SOURCES)
libsb_cfgP521CPc0sw_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CPc0sw_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CPc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CPc0sw_LDADD = libsb_cfgP521CPc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO521_A) libsb_cfgP521CPc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CPc1sw.a and securebootsim_cfgP521CPc1sw
libsb_cfgP521CPc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA521_SOURCES)
libsb_cfgP521CPc1sw_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CPc1sw_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CPc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CPc1sw_LDADD = libsb_cfgP521CPc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO521_A) libsb_cfgP521CPc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CPc2sw.a and securebootsim_cfgP521CPc2sw
libsb_cfgP521CPc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA521_SOURCES)
libsb_cfgP521CPc2sw_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CPc2sw_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CPc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CPc2sw_LDADD = libsb_cfgP521CPc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO521_A) libsb_cfgP521CPc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521IPc0sm.a and securebootsim_cfgP521IPc0sm
libsb_cfgP521IPc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP521IPc0sm_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP521IPc0sm_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP521IPc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP521IPc0sm_LDADD = libsb_cfgP521IPc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521IPc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521IPc1sm.a and securebootsim_cfgP521IPc1sm
libsb_cfgP521IPc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP521IPc1sm_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP521IPc1sm_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP521IPc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP521IPc1sm_LDADD = libsb_cfgP521IPc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521IPc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521IPc2sm.a and securebootsim_cfgP521IPc2sm
libsb_cfgP521IPc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP521IPc2sm_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP521IPc2sm_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP521IPc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP521IPc2sm_LDADD = libsb_cfgP521IPc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521IPc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CPc0sm.a and securebootsim_cfgP521CPc0sm
libsb_cfgP521CPc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP521CPc0sm_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CPc0sm_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CPc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CPc0sm_LDADD = libsb_cfgP521CPc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CPc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CPc1sm.a and securebootsim_cfgP521CPc1sm
libsb_cfgP521CPc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP521CPc1sm_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CPc1sm_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CPc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CPc1sm_LDADD = libsb_cfgP521CPc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CPc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CPc2sm.a and securebootsim_cfgP521CPc2sm
libsb_cfgP521CPc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP521CPc2sm_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CPc2sm_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CPc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CPc2sm_LDADD = libsb_cfgP521CPc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CPc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521IPc0pk.a and securebootsim_cfgP521IPc0pk
libsb_cfgP521IPc0pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH512_SOURCES)
libsb_cfgP521IPc0pk_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP521IPc0pk_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP521IPc0pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP521IPc0pk_LDADD = libsb_cfgP521IPc0pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521IPc0pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521IPc1pk.a and securebootsim_cfgP521IPc1pk
libsb_cfgP521IPc1pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH512_SOURCES)
libsb_cfgP521IPc1pk_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP521IPc1pk_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP521IPc1pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP521IPc1pk_LDADD = libsb_cfgP521IPc1pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521IPc1pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521IPc2pk.a and securebootsim_cfgP521IPc2pk
libsb_cfgP521IPc2pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(SB_SW_HASH512_SOURCES)
libsb_cfgP521IPc2pk_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP521IPc2pk_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE
securebootsim_cfgP521IPc2pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP521IPc2pk_LDADD = libsb_cfgP521IPc2pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521IPc2pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CPc0pk.a and securebootsim_cfgP521CPc0pk
libsb_cfgP521CPc0pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api_decrypt.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_aes_if.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES)
libsb_cfgP521CPc0pk_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CPc0pk_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CPc0pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CPc0pk_LDADD = libsb_cfgP521CPc0pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CPc0pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CPc1pk.a and securebootsim_cfgP521CPc1pk
libsb_cfgP521CPc1pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api_decrypt.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_aes_if.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES)
libsb_cfgP521CPc1pk_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CPc1pk_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CPc1pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CPc1pk_LDADD = libsb_cfgP521CPc1pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CPc1pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CPc2pk.a and securebootsim_cfgP521CPc2pk
libsb_cfgP521CPc2pk_a_SOURCES =  $(SB_PK_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_PK/src/sbpk_api_decrypt.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_hw_init.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_eip28_ecdsa_verify.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_aes_if.c \
    $(top_src)/SBLib/SB_PK/src/sbpk_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES)
libsb_cfgP521CPc2pk_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CPc2pk_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CPc2pk_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CPc2pk_LDADD = libsb_cfgP521CPc2pk.a $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CPc2pk.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXPc0sw.a and securebootsim_cfgP521CXPc0sw
libsb_cfgP521CXPc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA521_SOURCES)
libsb_cfgP521CXPc0sw_a_CPPFLAGS = $(LIBSB_HYBRID521AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CXPc0sw_CPPFLAGS = $(SECUREBOOTSIM521AES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXPc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXPc0sw_LDADD = libsb_cfgP521CXPc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXPc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXPc1sw.a and securebootsim_cfgP521CXPc1sw
libsb_cfgP521CXPc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA521_SOURCES)
libsb_cfgP521CXPc1sw_a_CPPFLAGS = $(LIBSB_HYBRID521AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CXPc1sw_CPPFLAGS = $(SECUREBOOTSIM521AES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXPc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXPc1sw_LDADD = libsb_cfgP521CXPc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXPc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXPc2sw.a and securebootsim_cfgP521CXPc2sw
libsb_cfgP521CXPc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA521_SOURCES)
libsb_cfgP521CXPc2sw_a_CPPFLAGS = $(LIBSB_HYBRID521AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CXPc2sw_CPPFLAGS = $(SECUREBOOTSIM521AES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXPc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXPc2sw_LDADD = libsb_cfgP521CXPc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXPc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXPKc0sw.a and securebootsim_cfgP521CXPKc0sw
libsb_cfgP521CXPKc0sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA521_SOURCES)
libsb_cfgP521CXPKc0sw_a_CPPFLAGS = $(LIBSB_HYBRID521KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CXPKc0sw_CPPFLAGS = $(SECUREBOOTSIM521KDKAES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXPKc0sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXPKc0sw_LDADD = libsb_cfgP521CXPKc0sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXPKc0sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXPKc1sw.a and securebootsim_cfgP521CXPKc1sw
libsb_cfgP521CXPKc1sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA521_SOURCES)
libsb_cfgP521CXPKc1sw_a_CPPFLAGS = $(LIBSB_HYBRID521KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CXPKc1sw_CPPFLAGS = $(SECUREBOOTSIM521KDKAES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXPKc1sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXPKc1sw_LDADD = libsb_cfgP521CXPKc1sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXPKc1sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXPKc2sw.a and securebootsim_cfgP521CXPKc2sw
libsb_cfgP521CXPKc2sw_a_SOURCES =  \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA521_SOURCES)
libsb_cfgP521CXPKc2sw_a_CPPFLAGS = $(LIBSB_HYBRID521KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CXPKc2sw_CPPFLAGS = $(SECUREBOOTSIM521KDKAES256_INCLUDES) -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXPKc2sw_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXPKc2sw_LDADD = libsb_cfgP521CXPKc2sw.a $(LIBSBHW_TARGET_SW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXPKc2sw.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CNc0sm.a and securebootsim_cfgP521CNc0sm
libsb_cfgP521CNc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP521CNc0sm_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP521CNc0sm_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CNc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CNc0sm_LDADD = libsb_cfgP521CNc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CNc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CNc1sm.a and securebootsim_cfgP521CNc1sm
libsb_cfgP521CNc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP521CNc1sm_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP521CNc1sm_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CNc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CNc1sm_LDADD = libsb_cfgP521CNc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CNc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CNc2sm.a and securebootsim_cfgP521CNc2sm
libsb_cfgP521CNc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP521CNc2sm_a_CPPFLAGS = $(LIBSB_HYBRID521_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP521CNc2sm_CPPFLAGS = $(SECUREBOOTSIM521_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CNc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CNc2sm_LDADD = libsb_cfgP521CNc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CNc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXPc0sm.a and securebootsim_cfgP521CXPc0sm
libsb_cfgP521CXPc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP521CXPc0sm_a_CPPFLAGS = $(LIBSB_HYBRID521AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CXPc0sm_CPPFLAGS = $(SECUREBOOTSIM521AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXPc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXPc0sm_LDADD = libsb_cfgP521CXPc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXPc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXPc1sm.a and securebootsim_cfgP521CXPc1sm
libsb_cfgP521CXPc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP521CXPc1sm_a_CPPFLAGS = $(LIBSB_HYBRID521AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CXPc1sm_CPPFLAGS = $(SECUREBOOTSIM521AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXPc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXPc1sm_LDADD = libsb_cfgP521CXPc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXPc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXPc2sm.a and securebootsim_cfgP521CXPc2sm
libsb_cfgP521CXPc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP521CXPc2sm_a_CPPFLAGS = $(LIBSB_HYBRID521AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CXPc2sm_CPPFLAGS = $(SECUREBOOTSIM521AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXPc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXPc2sm_LDADD = libsb_cfgP521CXPc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXPc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXPKc0sm.a and securebootsim_cfgP521CXPKc0sm
libsb_cfgP521CXPKc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP521CXPKc0sm_a_CPPFLAGS = $(LIBSB_HYBRID521KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CXPKc0sm_CPPFLAGS = $(SECUREBOOTSIM521KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXPKc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXPKc0sm_LDADD = libsb_cfgP521CXPKc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXPKc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXPKc1sm.a and securebootsim_cfgP521CXPKc1sm
libsb_cfgP521CXPKc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP521CXPKc1sm_a_CPPFLAGS = $(LIBSB_HYBRID521KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CXPKc1sm_CPPFLAGS = $(SECUREBOOTSIM521KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXPKc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXPKc1sm_LDADD = libsb_cfgP521CXPKc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXPKc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXPKc2sm.a and securebootsim_cfgP521CXPKc2sm
libsb_cfgP521CXPKc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c
libsb_cfgP521CXPKc2sm_a_CPPFLAGS = $(LIBSB_HYBRID521KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
securebootsim_cfgP521CXPKc2sm_CPPFLAGS = $(SECUREBOOTSIM521KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXPKc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXPKc2sm_LDADD = libsb_cfgP521CXPKc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXPKc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXNc0sm.a and securebootsim_cfgP521CXNc0sm
libsb_cfgP521CXNc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP521CXNc0sm_a_CPPFLAGS = $(LIBSB_HYBRID521AES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP521CXNc0sm_CPPFLAGS = $(SECUREBOOTSIM521AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXNc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXNc0sm_LDADD = libsb_cfgP521CXNc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXNc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXNc1sm.a and securebootsim_cfgP521CXNc1sm
libsb_cfgP521CXNc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP521CXNc1sm_a_CPPFLAGS = $(LIBSB_HYBRID521AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP521CXNc1sm_CPPFLAGS = $(SECUREBOOTSIM521AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXNc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXNc1sm_LDADD = libsb_cfgP521CXNc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXNc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXNc2sm.a and securebootsim_cfgP521CXNc2sm
libsb_cfgP521CXNc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP521CXNc2sm_a_CPPFLAGS = $(LIBSB_HYBRID521AES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP521CXNc2sm_CPPFLAGS = $(SECUREBOOTSIM521AES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXNc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXNc2sm_LDADD = libsb_cfgP521CXNc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXNc2sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXNKc0sm.a and securebootsim_cfgP521CXNKc0sm
libsb_cfgP521CXNKc0sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP521CXNKc0sm_a_CPPFLAGS = $(LIBSB_HYBRID521KDKAES256_INCLUDES) -DSBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
securebootsim_cfgP521CXNKc0sm_CPPFLAGS = $(SECUREBOOTSIM521KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXNKc0sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXNKc0sm_LDADD = libsb_cfgP521CXNKc0sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXNKc0sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXNKc1sm.a and securebootsim_cfgP521CXNKc1sm
libsb_cfgP521CXNKc1sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP521CXNKc1sm_a_CPPFLAGS = $(LIBSB_HYBRID521KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1
securebootsim_cfgP521CXNKc1sm_CPPFLAGS = $(SECUREBOOTSIM521KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXNKc1sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXNKc1sm_LDADD = libsb_cfgP521CXNKc1sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXNKc1sm.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP521CXNKc2sm.a and securebootsim_cfgP521CXNKc2sm
libsb_cfgP521CXNKc2sm_a_SOURCES =  $(SB_SM_DRIVER_SOURCES) \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBLib/SB_SM/src/sbsm_api_decrypt.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_hw_init.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_aes_if.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_asset.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_common.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_hash.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_publicdata.c \
    $(top_src)/SBLib/SB_SM/src/sbsm_eip130_system_info.c \
    $(top_src)/Framework/CLIB/src/c_lib.c
libsb_cfgP521CXNKc2sm_a_CPPFLAGS = $(LIBSB_HYBRID521KDKAES256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2
securebootsim_cfgP521CXNKc2sm_CPPFLAGS = $(SECUREBOOTSIM521KDKAES256_INCLUDES) -DSBSIM_ALLOCATOR_DMARESOURCE -DSBSIM_WITH_DECRYPT
securebootsim_cfgP521CXNKc2sm_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP521CXNKc2sm_LDADD = libsb_cfgP521CXNKc2sm.a $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW) $(LIBSBCRYPTO521_A) libsb_cfgP521CXNKc2sm.a $(OPT_LIBDEBUG_A)



#----------------------------------------------------------------------------
# libsb_cfgP*IP*s and libsb_cfgP*CP*s
#----------------------------------------------------------------------------

# The following configurations use SB_S

# Rules for libsb_cfgP224IPc0s.a and securebootsim_cfgP224IPc0s
libsb_cfgP224IPc0s_a_SOURCES = \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES) \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_stub_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c

libsb_cfgP224IPc0s_a_CPPFLAGS = $(LIBSB_SW224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=0 $(ABSTRACTION_INCLUDES)
securebootsim_cfgP224IPc0s_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) $(ABSTRACTION_INCLUDES)
securebootsim_cfgP224IPc0s_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc0s_LDADD = libsb_cfgP224IPc0s.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc1s.a and securebootsim_cfgP224IPc1s
libsb_cfgP224IPc1s_a_SOURCES = \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES) \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_stub_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c

libsb_cfgP224IPc1s_a_CPPFLAGS = $(LIBSB_SW224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 $(ABSTRACTION_INCLUDES)
securebootsim_cfgP224IPc1s_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) $(ABSTRACTION_INCLUDES)
securebootsim_cfgP224IPc1s_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc1s_LDADD = libsb_cfgP224IPc1s.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224IPc2s.a and securebootsim_cfgP224IPc2s
libsb_cfgP224IPc2s_a_SOURCES = \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES) \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_stub_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c

libsb_cfgP224IPc2s_a_CPPFLAGS = $(LIBSB_SW224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 $(ABSTRACTION_INCLUDES)
securebootsim_cfgP224IPc2s_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) $(ABSTRACTION_INCLUDES)
securebootsim_cfgP224IPc2s_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP224IPc2s_LDADD = libsb_cfgP224IPc2s.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc0s.a and securebootsim_cfgP224CPc0s
libsb_cfgP224CPc0s_a_SOURCES = \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(SB_SW_AES_SOURCES) \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES) \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api_decrypt.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_stub_publicdata.c \
    $(top_src)/SBCommon/SBIF_TOOLS/src/sbif_tools_convert.c \
    $(top_src)/Framework/CLIB/src/c_lib.c

libsb_cfgP224CPc0s_a_CPPFLAGS = $(LIBSB_SW224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=0 -DAES_IF_OMIT_KEYGEN -DAES_SUPPORT_ENCRYPT $(ABSTRACTION_INCLUDES)
securebootsim_cfgP224CPc0s_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_WITH_DECRYPT $(ABSTRACTION_INCLUDES)
securebootsim_cfgP224CPc0s_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc0s_LDADD = libsb_cfgP224CPc0s.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc1s.a and securebootsim_cfgP224CPc1s
libsb_cfgP224CPc1s_a_SOURCES = \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(SB_SW_AES_SOURCES) \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES) \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api_decrypt.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_stub_publicdata.c \
    $(top_src)/SBCommon/SBIF_TOOLS/src/sbif_tools_convert.c \
    $(top_src)/Framework/CLIB/src/c_lib.c

libsb_cfgP224CPc1s_a_CPPFLAGS = $(LIBSB_SW224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DAES_IF_OMIT_KEYGEN -DAES_SUPPORT_ENCRYPT $(ABSTRACTION_INCLUDES)
securebootsim_cfgP224CPc1s_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_WITH_DECRYPT $(ABSTRACTION_INCLUDES)
securebootsim_cfgP224CPc1s_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc1s_LDADD = libsb_cfgP224CPc1s.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP224CPc2s.a and securebootsim_cfgP224CPc2s
libsb_cfgP224CPc2s_a_SOURCES = \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(SB_SW_AES_SOURCES) \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA224_SOURCES) \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api_decrypt.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_stub_publicdata.c \
    $(top_src)/SBCommon/SBIF_TOOLS/src/sbif_tools_convert.c \
    $(top_src)/Framework/CLIB/src/c_lib.c

libsb_cfgP224CPc2s_a_CPPFLAGS = $(LIBSB_SW224_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DAES_IF_OMIT_KEYGEN -DAES_SUPPORT_ENCRYPT $(ABSTRACTION_INCLUDES)
securebootsim_cfgP224CPc2s_CPPFLAGS = $(SECUREBOOTSIM224_INCLUDES) -DSBSIM_WITH_DECRYPT $(ABSTRACTION_INCLUDES)
securebootsim_cfgP224CPc2s_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP224CPc2s_LDADD = libsb_cfgP224CPc2s.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc0s.a and securebootsim_cfgP256IPc0s
libsb_cfgP256IPc0s_a_SOURCES = \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES) \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_stub_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c

libsb_cfgP256IPc0s_a_CPPFLAGS = $(LIBSB_SW256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=0 $(ABSTRACTION_INCLUDES)
securebootsim_cfgP256IPc0s_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) $(ABSTRACTION_INCLUDES)
securebootsim_cfgP256IPc0s_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc0s_LDADD = libsb_cfgP256IPc0s.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc1s.a and securebootsim_cfgP256IPc1s
libsb_cfgP256IPc1s_a_SOURCES = \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES) \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_stub_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c

libsb_cfgP256IPc1s_a_CPPFLAGS = $(LIBSB_SW256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 $(ABSTRACTION_INCLUDES)
securebootsim_cfgP256IPc1s_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) $(ABSTRACTION_INCLUDES)
securebootsim_cfgP256IPc1s_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc1s_LDADD = libsb_cfgP256IPc1s.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256IPc2s.a and securebootsim_cfgP256IPc2s
libsb_cfgP256IPc2s_a_SOURCES = \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES) \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_stub_publicdata.c \
    $(top_src)/Framework/CLIB/src/c_lib.c

libsb_cfgP256IPc2s_a_CPPFLAGS = $(LIBSB_SW256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 $(ABSTRACTION_INCLUDES)
securebootsim_cfgP256IPc2s_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) $(ABSTRACTION_INCLUDES)
securebootsim_cfgP256IPc2s_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_verify.c
securebootsim_cfgP256IPc2s_LDADD = libsb_cfgP256IPc2s.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc0s.a and securebootsim_cfgP256CPc0s
libsb_cfgP256CPc0s_a_SOURCES = \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(SB_SW_AES_SOURCES) \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES) \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api_decrypt.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_stub_publicdata.c \
    $(top_src)/SBCommon/SBIF_TOOLS/src/sbif_tools_convert.c \
    $(top_src)/Framework/CLIB/src/c_lib.c

libsb_cfgP256CPc0s_a_CPPFLAGS = $(LIBSB_SW256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=0 -DAES_IF_OMIT_KEYGEN -DAES_SUPPORT_ENCRYPT $(ABSTRACTION_INCLUDES)
securebootsim_cfgP256CPc0s_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_WITH_DECRYPT $(ABSTRACTION_INCLUDES)
securebootsim_cfgP256CPc0s_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc0s_LDADD = libsb_cfgP256CPc0s.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc1s.a and securebootsim_cfgP256CPc1s
libsb_cfgP256CPc1s_a_SOURCES = \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(SB_SW_AES_SOURCES) \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES) \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api_decrypt.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_stub_publicdata.c \
    $(top_src)/SBCommon/SBIF_TOOLS/src/sbif_tools_convert.c \
    $(top_src)/Framework/CLIB/src/c_lib.c

libsb_cfgP256CPc1s_a_CPPFLAGS = $(LIBSB_SW256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=1 -DAES_IF_OMIT_KEYGEN -DAES_SUPPORT_ENCRYPT $(ABSTRACTION_INCLUDES)
securebootsim_cfgP256CPc1s_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_WITH_DECRYPT $(ABSTRACTION_INCLUDES)
securebootsim_cfgP256CPc1s_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc1s_LDADD = libsb_cfgP256CPc1s.a $(OPT_LIBDEBUG_A)

# Rules for libsb_cfgP256CPc2s.a and securebootsim_cfgP256CPc2s
libsb_cfgP256CPc2s_a_SOURCES = \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(SB_SW_AES_SOURCES) \
    $(SB_SW_HASH256_SOURCES) \
    $(SB_SW_ECDSA256_SOURCES) \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api_decrypt.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api.c \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_stub_publicdata.c \
    $(top_src)/SBCommon/SBIF_TOOLS/src/sbif_tools_convert.c \
    $(top_src)/Framework/CLIB/src/c_lib.c

libsb_cfgP256CPc2s_a_CPPFLAGS = $(LIBSB_SW256_INCLUDES) -DSBLIB_CFG_CERTIFICATES_MAX=2 -DAES_IF_OMIT_KEYGEN -DAES_SUPPORT_ENCRYPT $(ABSTRACTION_INCLUDES)
securebootsim_cfgP256CPc2s_CPPFLAGS = $(SECUREBOOTSIM256_INCLUDES) -DSBSIM_WITH_DECRYPT $(ABSTRACTION_INCLUDES)
securebootsim_cfgP256CPc2s_SOURCES = $(top_src)/SBLib/SBSIM/src/sbsim_decrypt_and_verify.c
securebootsim_cfgP256CPc2s_LDADD = libsb_cfgP256CPc2s.a $(OPT_LIBDEBUG_A)



securebootsim: $(SECUREBOOTSIM)
	cp -f $< $@

#----------------------------------------------------------------------------
# libtarget_versatile: Target Versatile FPGA
#----------------------------------------------------------------------------

if ENABLE_VERSATILE

libtarget_versatile_a_CPPFLAGS = \
    $(CONFIGURATION_INCLUDES) \
    -I$(top_src)/Framework/PUBDEFS/incl \
    -I$(top_src)/Framework/IMPLDEFS/incl \
    -I$(top_src)/Framework/SPAL_API/incl \
    -I$(top_src)/Framework/CLIB/incl \
    $(DMARES_INCLUDE) \
    -I$(top_src)/Integration/UMDevXS/UserPart/incl \
    $(ABSTRACTION_INCLUDES) \
    -I$(top_src)/Kit/DriverFramework/v4/Device_API/incl \
    -I$(top_src)/Kit/DriverFramework/v4/DMAResource_API/incl \
    -I$(top_src)/Kit/Log/incl \
    -I$(top_src)/Kit/Log/src/safezone

libtarget_versatile_a_SOURCES = \
    $(top_src)/Integration/DriverFramework_v4_impl/src/hwpal_dmares_umdevxs.c \
    $(top_src)/Integration/DriverFramework_v4_impl/src/hwpal_dmares_addr_cm_fpga.c \
    $(top_src)/Integration/DriverFramework_v4_impl/src/hwpal_device_umdevxs.c

endif

#----------------------------------------------------------------------------
# libumdevxs: UMDevXS Proxy implementation (needed by Target Versatile FPGA)
#----------------------------------------------------------------------------

if ENABLE_VERSATILE

libumdevxs_a_CPPFLAGS = \
    $(CONFIGURATION_INCLUDES) \
    -I$(top_src)/Framework/PUBDEFS/incl \
    -I$(top_src)/Framework/IMPLDEFS/incl \
    $(ABSTRACTION_INCLUDES) \
    -I$(top_src)/Kit/Log/incl \
    -I$(top_src)/Kit/Log/src/safezone \
    -I$(top_src)/Integration/UMDevXS/UserPart/incl \
    -I$(top_src)/Integration/UMDevXS/KernelPart/incl

libumdevxs_a_SOURCES = \
    $(top_src)/Integration/UMDevXS/UserPart/src/umdevxsproxy.c

endif

#----------------------------------------------------------------------------
# libtarget_bare: Target Barebones
#----------------------------------------------------------------------------

if ENABLE_BARE

libtarget_bare_a_CPPFLAGS = \
    $(CONFIGURATION_INCLUDES) \
    -I$(top_src)/Framework/PUBDEFS/incl \
    -I$(top_src)/Framework/IMPLDEFS/incl \
    -I$(top_src)/Framework/SPAL_API/incl \
    -I$(top_src)/Framework/CLIB/incl \
    $(DMARES_INCLUDE) \
    -I$(top_src)/Kit/DriverFramework/v4_bare/incl \
    -I$(top_src)/Kit/DriverFramework/v4/Device_API/incl \
    -I$(top_src)/Kit/DriverFramework/v4/DMAResource_API/incl \
    -I$(top_src)/Kit/Log/incl \
    -I$(top_src)/Kit/Log/src/safezone

libtarget_bare_a_SOURCES = \
    $(top_src)/Integration/DriverFramework_v4_bare/src/hwpal_device_bare.c \
    $(top_src)/Integration/DriverFramework_v4_bare/src/hwpal_dmares_bare.c

endif



#----------------------------------------------------------------------------
# Software Only Secure Boot tests
#----------------------------------------------------------------------------

# A simple test case for Secure Boot P-224.

SBSW_CPPFLAGS = \
    $(CONFIGURATION_INCLUDES) \
    -I$(top_src)/Framework/PUBDEFS/incl \
    -I$(top_src)/Framework/IMPLDEFS/incl \
    -I$(top_src)/Framework/CLIB/incl \
    -I$(top_src)/SFZUTF/incl \
    -I$(top_src)/SBLib/SB_API/incl \
    -I$(top_src)/SBLib/SB_HYBRID/src \
    -I$(top_src)/SBCommon/SBIF_API/incl \
    -I$(top_src)/SBCommon/SBIF_TOOLS/incl \
    -I$(top_src)/SBCommon/SB_SW_CRYPTO/incl \
    -I$(top_src)/SBLib/SB_SIMPLE/src

libsbsw224_a_SOURCES = \
    $(top_src)/SBLib/SB_SIMPLE/src/sb_api.c

libsbsw224_a_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    -I$(top_src)/SBCommon/Config \
    $(SBSW_CPPFLAGS)

# sb*test224

sbstest224_LDADD = \
    libsb_cfgP224CPc0s.a \
    $(check_LIBRARIES)

sbstest224_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    -I$(top_src)/SBCommon/Config \
    $(ABSTRACTION_INCLUDES) \
    $(SBSW_CPPFLAGS) \
   -DEXCLUDE_BLX_TESTS

sbstest224_SOURCES = \
    $(top_src)/SBLib/SB_SW_TEST/src/sbswtestcp224.c


sbswtest224_LDADD = \
    libsb_cfgP224CPc0sw.a \
    $(check_LIBRARIES)

sbswtest224_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    -I$(top_src)/SBCommon/Config \
    $(ABSTRACTION_INCLUDES) \
    $(SBSW_CPPFLAGS)

sbswtest224_SOURCES = \
    $(top_src)/SBLib/SB_SW_TEST/src/sbswtestcp224.c

sbsmtest224_LDADD = \
    libsb_cfgP224CNc0sm.a \
    $(check_LIBRARIES) \
    $(LIBSBHW_TARGET_SM) $(LIBSB_GENERAL_HW)

sbsmtest224_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    -I$(top_src)/SBCommon/Config \
    $(CONFIGURATION_INCLUDES) \
    $(ABSTRACTION_INCLUDES) \
    -I$(top_src)/Kit/DriverFramework/v4/Device_API/incl \
    -I$(top_src)/Kit/DriverFramework/v4/DMAResource_API/incl \
    $(DMARES_INCLUDE) \
    -I$(top_src)/Integration/UMDevXS/UserPart/incl \
    $(SBSW_CPPFLAGS) \
    -DSB_ALLOCATOR_DMARESOURCE

sbsmtest224_SOURCES = \
    $(top_src)/SBLib/SB_SW_TEST/src/sbswtestcp224.c

sbcmtest224_LDADD = \
    libsb_cfgP224CNc0cm.a \
    $(check_LIBRARIES) \
    $(LIBSBHW_TARGET_CM) $(LIBSB_GENERAL_HW)

sbcmtest224_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    -I$(top_src)/SBCommon/Config \
    $(CONFIGURATION_INCLUDES) \
    $(ABSTRACTION_INCLUDES) \
    -I$(top_src)/Kit/DriverFramework/v4/Device_API/incl \
    -I$(top_src)/Kit/DriverFramework/v4/DMAResource_API/incl \
    $(DMARES_INCLUDE) \
    -I$(top_src)/Integration/UMDevXS/UserPart/incl \
    $(SBSW_CPPFLAGS) \
    -DSB_ALLOCATOR_DMARESOURCE

sbcmtest224_SOURCES = \
    $(top_src)/SBLib/SB_SW_TEST/src/sbswtestcp224.c

sbcmpktest224_LDADD = \
    libsb_cfgP224CNc0cmpk.a \
    $(check_LIBRARIES) \
    $(LIBSBHW_TARGET_CMPK) $(LIBSB_GENERAL_HW)

sbcmpktest224_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    -I$(top_src)/SBCommon/Config \
    $(CONFIGURATION_INCLUDES) \
    $(ABSTRACTION_INCLUDES) \
    -I$(top_src)/Kit/DriverFramework/v4/Device_API/incl \
    -I$(top_src)/Kit/DriverFramework/v4/DMAResource_API/incl \
    $(DMARES_INCLUDE) \
    -I$(top_src)/Integration/UMDevXS/UserPart/incl \
    $(SBSW_CPPFLAGS) \
    -DSB_ALLOCATOR_DMARESOURCE

sbcmpktest224_SOURCES = \
    $(top_src)/SBLib/SB_SW_TEST/src/sbswtestcp224.c

sbpetest224_LDADD = \
    libsb_cfgP224CPc0pe.a \
    $(check_LIBRARIES) \
    $(LIBSBHW_TARGET_PE) $(LIBSB_GENERAL_HW)

sbpetest224_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    -I$(top_src)/SBCommon/Config \
    $(CONFIGURATION_INCLUDES) \
    $(ABSTRACTION_INCLUDES) \
    -I$(top_src)/Kit/DriverFramework/v4/Device_API/incl \
    -I$(top_src)/Kit/DriverFramework/v4/DMAResource_API/incl \
    $(DMARES_INCLUDE) \
    -I$(top_src)/Integration/UMDevXS/UserPart/incl \
    $(SBSW_CPPFLAGS) \
    -DSB_ALLOCATOR_DMARESOURCE

sbpetest224_SOURCES = \
    $(top_src)/SBLib/SB_SW_TEST/src/sbswtestcp224.c

sbpepktest224_LDADD = \
    libsb_cfgP224CPc0pepk.a \
    $(check_LIBRARIES) \
    $(LIBSBHW_TARGET_PEPK) $(LIBSB_GENERAL_HW)

sbpepktest224_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    -I$(top_src)/SBCommon/Config \
    $(CONFIGURATION_INCLUDES) \
    $(ABSTRACTION_INCLUDES) \
    -I$(top_src)/Kit/DriverFramework/v4/Device_API/incl \
    -I$(top_src)/Kit/DriverFramework/v4/DMAResource_API/incl \
    $(DMARES_INCLUDE) \
    -I$(top_src)/Integration/UMDevXS/UserPart/incl \
    $(SBSW_CPPFLAGS) \
    -DSB_ALLOCATOR_DMARESOURCE

sbpepktest224_SOURCES = \
    $(top_src)/SBLib/SB_SW_TEST/src/sbswtestcp224.c

sbpktest224_LDADD = \
    libsb_cfgP224CPc0pk.a \
    $(check_LIBRARIES) \
    $(LIBSBHW_TARGET_PK) $(LIBSB_GENERAL_HW)

sbpktest224_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    -I$(top_src)/SBCommon/Config \
    $(ABSTRACTION_INCLUDES) \
    $(SBSW_CPPFLAGS)

sbpktest224_SOURCES = \
    $(top_src)/SBLib/SB_SW_TEST/src/sbswtestcp224.c

#----------------------------------------------------------------------------
# libsfzutf
#----------------------------------------------------------------------------

libsfzutf_a_CPPFLAGS = \
    $(CONFIGURATION_INCLUDES) \
    -I$(top_src)/Framework/PUBDEFS/incl \
    -I$(top_src)/Framework/IMPLDEFS/incl \
    -I$(top_src)/SFZUTF/incl \
    -I$(top_src)/Framework/SPAL_API/incl

libsfzutf_a_SOURCES = \
    $(top_src)/SFZUTF/src/sfzutf.c \
    $(top_src)/SFZUTF/src/sfzutf_main_stdio.c \
    $(top_src)/SFZUTF/src/sfzutf_interactive.c \
    $(top_src)/SFZUTF/src/sfzutf-utils.c

if ENABLE_STACK
libsfzutf_a_SOURCES += $(top_src)/SFZUTF/src/sfzutf-stack.c
endif
if ENABLE_HEAP
libsfzutf_a_SOURCES += $(top_src)/SFZUTF/src/sfzutf-heap.c
endif
if ENABLE_PERF
libsfzutf_a_SOURCES += $(top_src)/SFZUTF/src/sfzutf-perf.c
endif


#----------------------------------------------------------------------------
# libdebug
#----------------------------------------------------------------------------

libdebug_a_CPPFLAGS = \
    $(CONFIGURATION_INCLUDES) \
    -I$(top_src)/Framework/PUBDEFS/incl \
    -I$(top_src)/Framework/IMPLDEFS/incl

libdebug_a_SOURCES = \
    $(top_src)/Framework/DEBUG/src/debug_printf.c \
    $(top_src)/Framework/DEBUG/src/debug_abort.c

if ENABLE_DEBUG
libdebug_a_CPPFLAGS += \
    $(ABSTRACTION_INCLUDES) \
    -I$(top_src)/Kit/Log/incl \
    -I$(top_src)/Kit/Log/src/safezone

libdebug_a_SOURCES += $(top_src)/Kit/Log/src/safezone/log_safezone.c
endif

#----------------------------------------------------------------------------
# updatetool
#----------------------------------------------------------------------------
updatetool_CPPFLAGS = \
    -I$(top_src)/SBCommon/Config/ECC224 \
    $(LIBSB_SW_INCLUDES) \
    -I$(top_src)/Kit/DriverFramework/v4/Basic_Defs/incl \
    -I$(top_src)/Kit/DriverFramework/v4/CLib_Abstraction/incl \
    -I$(top_src)/Kit/DriverFramework/v4/Device_API/incl \
    -DSBLIB_CFG_CERTIFICATES_MAX=8 -DSBSWCRYPTO_CF_FULL_FUNCTIONALITY -DAES_SUPPORT_ENCRYPT \
    -DuECC_SUPPORTS_secp224r1=1 -DuECC_SUPPORTS_secp256r1=1 -DuECC_SUPPORTS_secp384r1=1 -DuECC_SUPPORTS_secp521r1=1

# The updatetool is based on the same files as the Secure Boot Configuration has.
# However it requires key generation capability also.
updatetool_SOURCES = \
    $(top_src)/SBLib/UPDATE_TOOL/src/updatetool.c \
    $(SBIF_ATTRIBUTE_SUPPORT_SOURCES) \
    $(top_src)/SBCommon/SBIF_TOOLS/src/sbif_tools_convert.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_api.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_hw_init.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
    $(top_src)/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
    $(SB_SW_HASH512_SOURCES) \
    $(SB_SW_ECDSA_SOURCES) \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/nist-sp-chain.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/nist-health-test.c \
    $(top_src)/SBCommon/SB_SW_CRYPTO/src/nist-sp-800-90.c \
    $(top_src)/Framework/CLIB/src/c_lib.c

updatetool_LDADD = \
    libdebug.a

#----------------------------------------------------------------------------
# sbtest_publicdata
#----------------------------------------------------------------------------

sbtest_publicdata_sw_CPPFLAGS = \
    $(CONFIGURATION_INCLUDES) \
    -I$(top_src)/SBCommon/Config \
    -I$(top_src)/Framework/PUBDEFS/incl \
    -I$(top_src)/Framework/IMPLDEFS/incl \
    -I$(top_src)/Framework/CLIB/incl \
    -I$(top_src)/SFZUTF/incl \
    -I$(top_src)/SBLib/SB_API/incl \
    -I$(top_src)/SBLib/SB_HYBRID/src \
    -I$(top_src)/SBCommon/SBIF_API/incl \
    -I$(top_src)/SBCommon/SBIF_TOOLS/incl \
    -I$(top_src)/SBCommon/SB_SW_CRYPTO/incl

sbtest_publicdata_sw_SOURCES = \
    $(top_src)/SBLib/SB_TEST/src/sbtest_publicdata.c

sbtest_publicdata_sw_LDADD = \
    libsb_cfgP224IPc0sw.a \
    $(LIBSBIF_A) \
    $(LIBSBCRYPTO_A) \
    $(LIBSBIF_TOOLS_KGF_A) \
    libsb_cfgP224IPc0sw.a \
    libdebug.a

sbtest_publicdata_cm_CPPFLAGS = \
    $(sbtest_publicdata_sw_CPPFLAGS) \
    $(ABSTRACTION_INCLUDES) \
    -I$(top_src)/Kit/DriverFramework/v4/Device_API/incl \
    -I$(top_src)/Kit/DriverFramework/v4/DMAResource_API/incl \
    $(DMARES_INCLUDE) \
    -I$(top_src)/Integration/UMDevXS/UserPart/incl \
    -I$(top_src)/Kit/EIP123_SL/incl \
    -I$(top_src)/Kit/EIP123_CM_Tokens/incl \
    -DSBSIM_ALLOCATOR_DMARESOURCE

sbtest_publicdata_cm_SOURCES = \
    $(sbtest_publicdata_sw_SOURCES)

sbtest_publicdata_cm_LDADD = \
    $(LIBSBPD_CM_A) \
    $(LIBSBIF_A) \
    $(LIBSBCRYPTO_A) \
    $(LIBSBIF_TOOLS_KGF_A) \
    $(LIBSBHW_TARGET_CM) \
    libdebug.a \
    $(LIBSB_GENERAL_HW)

sbtest_publicdata_sm_CPPFLAGS = \
    $(sbtest_publicdata_sw_CPPFLAGS) \
    $(ABSTRACTION_INCLUDES) \
    -I$(top_src)/Kit/DriverFramework/v4/Device_API/incl \
    -I$(top_src)/Kit/DriverFramework/v4/DMAResource_API/incl \
    $(DMARES_INCLUDE) \
    -I$(top_src)/Integration/UMDevXS/UserPart/incl \
    -I$(top_src)/Kit/EIP130/MailboxControl/incl \
    -I$(top_src)/Kit/EIP130/TokenHelper/incl \
    -DSBSIM_ALLOCATOR_DMARESOURCE

sbtest_publicdata_sm_SOURCES = \
    $(sbtest_publicdata_sw_SOURCES)

sbtest_publicdata_sm_LDADD = \
    libsb_cfgP224IPc0sm.a \
    $(LIBSBIF_A) \
    $(LIBSBCRYPTO_A) \
    $(LIBSBIF_TOOLS_KGF_A) \
    $(LIBSBHW_TARGET_SM) \
    libsb_cfgP224IPc0sm.a \
    libdebug.a \
    $(LIBSB_GENERAL_HW)

#----------------------------------------------------------------------------

# Notice: nobuffer.so is compiled with non-automake rule
nobuffer.so: $(top_src)/SFZUTF/util/nobuffer.c
	$(CC) $(CFLAGS) -shared -o $@ $<

# Flags & rules for running test runner
TESTRUNNER_FLAGS = "--timeout=600"

list_check:
	for test in $(check_PROGRAMS); \
	do echo '***' $$test;./$$test --list 2>&1; done

# Test runner (for single tests)
%.xml: %
	echo $(TESTRUNNER_PRE) \
            $(top_src)/SFZUTF/scripts/testrunner.pl \
            $(TESTRUNNER_FLAGS) \
            --xmllog=$@ \
            $<; \
        $(TESTRUNNER_PRE) \
            $(top_src)/SFZUTF/scripts/testrunner.pl \
            $(TESTRUNNER_FLAGS) \
            --xmllog=$@ \
            $<

run_check: $(check_PROGRAMS)
	@ok=0; \
    count=0; \
    for test in $(check_PROGRAMS); \
    do \
        count=$$((count+1)); \
        echo $(TESTRUNNER_PRE) \
            $(top_src)/SFZUTF/scripts/testrunner.pl \
            $(TESTRUNNER_FLAGS) \
            --xmllog=$$test.xml \
            $$test; \
        $(TESTRUNNER_PRE) \
            $(top_src)/SFZUTF/scripts/testrunner.pl \
            $(TESTRUNNER_FLAGS) \
            --xmllog=$$test.xml \
            $$test; \
        RES=$$?; \
        if [ "$$RES" -eq "0" ]; then \
            ok=$$((ok+1)); \
        fi; \
        if [ "$$RES" -eq "255" ]; then \
            echo "Test suite execution terminated"; \
            exit 1; \
        fi; \
    done; \
    echo "*** $$count TEST SUITES: $$ok/$$count SUCCESSFUL"

run_check_ssh: $(check_PROGRAMS)
	@ok=0; \
    count=0; \
    for test in $(check_PROGRAMS); \
    do \
        count=$$((count+1)); \
        scp $$test $(SSH_TARGET):/tmp/test; \
        echo \
            $(TESTRUNNER_PRE) \
            $(top_src)/SFZUTF/scripts/testrunner.pl \
            $(TESTRUNNER_FLAGS) \
            --interpreter=ssh \
            --interpreter-opts="-t -t $(SSH_TARGET)" \
            --xmllog=$$test.xml \
            /tmp/test \
            '#'$$test; \
        $(TESTRUNNER_PRE) \
            $(top_src)/SFZUTF/scripts/testrunner.pl \
            $(TESTRUNNER_FLAGS) \
            --interpreter=ssh \
            --interpreter-opts="-t -t $(SSH_TARGET)" \
            --xmllog=$$test.xml \
            /tmp/test; \
        RES=$$?; \
        if [ "$$RES" -eq "0" ]; then \
            ok=$$((ok+1)); \
        fi; \
        if [ "$$RES" -eq "255" ]; then \
            echo "Test suite execution terminated"; \
            exit 1; \
        fi; \
    done; \
    echo "*** $$count TEST SUITES: $$ok/$$count SUCCESSFUL"

#----------------------------------------------------------------------------
# Clean up
#----------------------------------------------------------------------------

distclean-local:
	rm -f aclocal.m4 compile configure install-sh Makefile.in missing depcomp config.guess config.sub
	rm -rf autom4te.cache
	rm -f *.gcno *.gcda *.xml
	rm -f libsb_*.a libtarget_*.a

# end of file Makefile.am
