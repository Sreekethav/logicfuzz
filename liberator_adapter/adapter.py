"""
Transform LogicFuzz data source to Liberator API model

Uses Clang/LLVM for API extraction:
   - Requires benchmark object and source code access
   - Extracts flag, size, const information directly from source

Note: FuzzIntrospector dependency has been removed for internal project support.
"""
from typing import Dict, List, Set, Optional
from liberator_adapter.common.api import Api, Arg
from liberator_adapter.dependency import DependencyGraph, TypeDependencyGraphGenerator

class LiberatorAPIAdapter:
    """
    Adapter to transform LogicFuzz data source to Liberator API model.

    Uses Clang/LLVM for API extraction (FuzzIntrospector dependency removed).
    """

    def __init__(self, project_name: str, benchmark, use_clang_llvm: bool = True):
        """
        Initialize adapter

        Args:
            project_name: project name
            benchmark: benchmark object (required for Clang/LLVM extraction)
            use_clang_llvm: kept for backward compatibility, always True

        Note:
            FuzzIntrospector dependency has been removed.
            All API extraction now uses Clang/LLVM directly.
        """
        self.project_name = project_name
        self.use_clang_llvm = True  # Always use Clang/LLVM
        self.api_cache: Dict[str, Api] = {}
        self.last_metadata: Dict = {}

        if not benchmark:
            raise ValueError("benchmark is required for API extraction")
        from liberator_adapter.extractors.hybrid_extractor import HybridAPIExtractor
        self.hybrid_extractor = HybridAPIExtractor(benchmark)
        
    def convert_to_liberator_api(
        self,
        function_signature: str,
        api_context: Optional[Dict] = None
    ) -> Optional[Api]:
        """
        Transform function information to Liberator API object using Clang/LLVM.

        Args:
            function_signature: function signature (e.g. "int curl_easy_setopt(CURL *, int, ...)")
            api_context: ignored (kept for backward compatibility)

        Returns:
            Api object, return None if conversion fails
        """
        func_name = self._extract_function_name(function_signature)
        if not func_name:
            return None

        # check cache
        if func_name in self.api_cache:
            return self.api_cache[func_name]

        # get API from hybrid extractor
        api = self.hybrid_extractor.get_api(func_name)
        if api:
            self.api_cache[func_name] = api
        return api
    
    def extract_all_apis(
        self,
        function_signatures: Optional[List[str]] = None,
        include_dir: Optional[str] = None,
        public_headers_file: Optional[str] = None,
        bc_file: Optional[str] = None,
        compile_project: bool = True
    ) -> Dict[str, Api]:
        """
        Extract all APIs using Clang/LLVM.

        Args:
            function_signatures: function signatures to extract (optional)
            include_dir: include directory (optional)
            public_headers_file: public headers file list (optional)
            bc_file: bitcode file path (optional)
            compile_project: whether to compile project (if bc_file is not provided)

        Returns:
            Dictionary of function name to Api object
        """
        apis = self.hybrid_extractor.extract(
            function_signatures=function_signatures,
            include_dir=include_dir,
            public_headers_file=public_headers_file,
            bc_file=bc_file,
            compile_project=compile_project
        )
        # Cache recent metadata (paths)
        try:
            self.last_metadata = self.hybrid_extractor.get_last_metadata() or {}
        except Exception:
            self.last_metadata = {}
        return apis
    
    def _extract_function_name(self, signature: str) -> Optional[str]:
        """Extract function name from function signature"""
        import re
        match = re.search(r'\b([a-zA-Z_][a-zA-Z0-9_]*(?:_[a-zA-Z0-9_]+)*)\s*\(', signature)
        return match.group(1) if match else None
    
    def _determine_flag(self, param: Dict) -> str:
        """Determine parameter flag (ref/val)"""
        param_type = param.get('type', '')
        if '*' in param_type or '[' in param_type:
            return 'ref'
        return 'val'
    
    def _determine_size(self, param: Dict) -> int:
        """Determine parameter size (in bytes)"""
        param_type = param.get('type', '')
        if not param_type:
            return 0
        
        # try to use DataLayout to get type size
        try:
            from liberator_adapter.common.datalayout import DataLayout
            # note: DataLayout returns bits, need to convert to bytes
            size_bits = DataLayout.instance().infer_type_size(param_type)
            return size_bits // 8  # convert to bytes
        except:
            # if DataLayout is not initialized or type is unknown, use simple heuristic rule
            if '*' in param_type:
                return 8  # pointer is 8 bytes on 64-bit system
            elif 'int' in param_type:
                if 'long' in param_type:
                    return 8
                elif 'short' in param_type:
                    return 2
                else:
                    return 4
            elif 'char' in param_type:
                return 1
            elif 'float' in param_type:
                return 4
            elif 'double' in param_type:
                return 8
            else:
                return 0  # unknown type
    
    def _determine_const(self, param: Dict) -> List[bool]:
        """Determine const modifier"""
        param_type = param.get('type', '')
        return ['const' in param_type]
    
    def _is_vararg(self, signature: str) -> bool:
        """Check if it is a variadic function"""
        return '...' in signature or ', ...' in signature
    
    def _extract_namespace(self, func_name: str) -> List[str]:
        """Extract namespace (based on function name prefix)"""
        parts = func_name.split('_')
        if len(parts) > 1:
            return parts[:-1]  # except the last part (function name)
        return []
    
    def cleanup(self):
        """Clean up resources"""
        if self.hybrid_extractor:
            self.hybrid_extractor.cleanup()

