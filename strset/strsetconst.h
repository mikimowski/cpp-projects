#ifndef __STRSETCONST_H__
#define __STRSETCONST_H__

#ifdef __cplusplus
namespace jnp1 {
    extern "C" {
#endif

    /**
    * Wywołana przez użytkownika zwraca identyfikator zbioru, którego nie można modyfikować i który zawiera
    * jeden element: napis "42". Zbiór jest tworzony przy pierwszym wywołaniu tej
    * funkcji i wtedy zostaje ustalony jego numer id.
    */
    unsigned long strset42();

#ifdef __cplusplus
    }
}
#endif

#endif // __STRSETCONST_H__
