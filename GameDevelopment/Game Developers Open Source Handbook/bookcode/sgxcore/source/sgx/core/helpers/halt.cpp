#include "sgx/core/types/types.hpp"
#include "sgx/core/helpers/trace.hpp"

// We used a function because when you break the code
// you can see its an explciity sgxHaltProcessor() and not an artbirary
// loop spinning.
void sgxHaltProcessor()
{
    // Use {} instead of ; to avoid warnings
    while (1){} 
}
