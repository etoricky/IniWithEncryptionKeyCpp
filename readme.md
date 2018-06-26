Purpose
=======

Use C++ to read INI file and for keys with configured name such as "Password" the value is encrypted in another INI item.

Input INI
=========

    Server=localhost:443
    Login=1
    Password=manager
    SqlType=MySQL
    SqlServer=localhost
    SqlLogin=root
    SqlPassword=
    SqlDB=MT4
    SqlCodePage=0
    PeriodPrices=30
    PeriodOpenTrades=180
    PeriodMargins=60
    SyncDaily=0
    ExternalUpdate=


Output INI
==========

    Server=localhost:443
    Login=1
    Password=
    PasswordHash=eczpg1jJ9j==
    SqlType=MySQL
    SqlServer=localhost
    SqlLogin=root
    SqlPassword=
    SqlPasswordHash=XjzncLLX9z==
    SqlDB=MT4
    SqlCodePage=0
    PeriodPrices=30
    PeriodOpenTrades=180
    PeriodMargins=60
    SyncDaily=0
    ExternalUpdate=
