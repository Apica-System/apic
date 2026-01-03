use apica_common::bytecodes::{ApicaBuiltinFunctionBytecode, ApicaTypeBytecode};
use apica_common::values::value::Value;

pub struct ParameterInfo {
    is_args: bool,
    param_type: Option<ApicaTypeBytecode>,
    default_value: Option<Value>,
}

impl ParameterInfo {
    pub fn init(is_args: bool, param_type: Option<ApicaTypeBytecode>, default_value: Option<Value>) -> ParameterInfo {
        ParameterInfo { is_args, param_type, default_value }
    }
    
    pub fn is_args(&self) -> bool {
        self.is_args
    }
    
    pub fn get_param_type(&self) -> &Option<ApicaTypeBytecode> {
        &self.param_type
    }
    
    pub fn get_default_value(&self) -> &Option<Value> {
        &self.default_value
    }
}

pub struct BuiltinFuncInfo {
    bytecode: ApicaBuiltinFunctionBytecode,
    parameters: Vec<ParameterInfo>,
    required: usize,
}

impl BuiltinFuncInfo {
    pub fn init(bytecode: ApicaBuiltinFunctionBytecode, parameters: Vec<ParameterInfo>, required: usize) -> BuiltinFuncInfo {
        BuiltinFuncInfo { bytecode, parameters, required }
    }
    
    pub fn get_bytecode(&self) -> ApicaBuiltinFunctionBytecode {
        self.bytecode
    }
    
    pub fn get_parameters(&self) -> &Vec<ParameterInfo> {
        &self.parameters
    }
    
    pub fn get_required(&self) -> usize {
        self.required
    }
}