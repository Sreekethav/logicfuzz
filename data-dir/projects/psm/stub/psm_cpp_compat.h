/* Force C linkage for all PSM / AUTOSAR  headers when compiled as C++.
 * This file is force-included (-include) when building LLM-generated
 * fuzz targets that use C++ features (FuzzedDataProvider, etc.).
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "Std_Types.h"
#include "psm_api.h"
#include "psm_internal.h"
#include "psm_abstract.h"

#ifdef __cplusplus
}
#endif

/* FuzzedDataProvider::ConsumeIntegral<T> requires std::is_integral_v<T>.
 * PSM enum types (Psm_tenPsId, Psm_tenPsState, …) are C enums, which
 * are NOT integral in C++.  The sed rewrites in build.sh handle this
 * by replacing ConsumeIntegral<EnumType>() with a cast-through-int pattern.
 */
