#pragma once

enum ApicaBytecode : unsigned long long {
    AB_EndOfFile =          0x00000000,
    AB_EndOfBlock =         0x00000001,
    AB_EntryPoint =         0x00000002,
    AB_BuiltinFuncCall =    0x00000003,
    AB_Literal =            0x00000004,
    AB_Global =             0x00000005,
    AB_Compound =           0x00000006,
    AB_VarConstCall =       0x00000007,
    AB_VarConstDecl =       0x00000008
};

enum ApicaTypeBytecode : unsigned long long {
    ATB_Null =      0x00000000,
    ATB_Any =       0x00000001,
    ATB_I8 =        0x00000002,
    ATB_I16 =       0x00000003,
    ATB_I32 =       0x00000004,
    ATB_I64 =       0x00000005,
    ATB_U8 =        0x00000006,
    ATB_U16 =       0x00000007,
    ATB_U32 =       0x00000008,
    ATB_U64 =       0x00000009,
    ATB_F32 =       0x0000000a,
    ATB_F64 =       0x0000000b,
    ATB_Bool =      0x0000000c,
    ATB_Char =      0x0000000d,
    ATB_String =    0x0000000e,
    ATB_Error =     0x0000000f
};

enum ApicaEntrypointBytecode : unsigned long long {
    AEB_Init =      0x00000000,
    AEB_Update =    0x00000001,
    AEB_Quit =      0x00000002
};

enum ApicaBuiltinFuncCallBytecode : unsigned long long {
    AFB_Quit =          0x00000000,
    AFB_LogInfo =       0x00000001,
    AFB_LognInfo =      0x00000002,
    AFB_LogSuccess =    0x00000003,
    AFB_LognSuccess =   0x00000004,
    AFB_LogWarning =    0x00000005,
    AFB_LognWarning =   0x00000006,
    AFB_LogError =      0x00000007,
    AFB_LognError =     0x00000008
};