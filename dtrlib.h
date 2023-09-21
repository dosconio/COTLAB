// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio

dnode* Dtr_system(dnode* const callinfo);

dnode* Dtr_load(dnode* const callinfo);

dnode* Dtr_dbg_test(dnode* const callinfo);

dnode* Dtr_int(dnode* const callinfo);

dnode* DtrASSIGN(dnode* const callinfo);// =

// ---- ARITH ----

dnode* DtrPREPOSI(dnode* const callinfo);

dnode* DtrPRENEGA(dnode* const callinfo);

dnode* DtrARIADD(dnode* const callinfo);

dnode* DtrARISUB(dnode* const callinfo);

dnode* DtrARIMUL(dnode* const callinfo);

dnode* DtrARIDIV(dnode* const callinfo);

dnode* DtrARIREM(dnode* const callinfo);

dnode* DtrARIPOW(dnode* const callinfo);
