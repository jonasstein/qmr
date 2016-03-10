//some debug macros

#define _DEBUG

#ifdef _DEBUG
#define SHOW(a) std::cout << #a << ": 0x" << std::hex << (a) << std::endl
#else
#define SHOW(a)
#endif

#ifdef _DEBUG
#define SHOWdec(a) std::cout << #a << ": " << std::dec << (a) << std::endl
#else
#define SHOWdec(a)
#endif
