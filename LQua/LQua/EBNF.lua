chunk -> {statement[;]}

statement-> 
				var_or_func[assignment] 
			| 	if_stat 
			| 	while_stat 
			| 	for_stat 
			| 	local_stat 
			| 	func_stat 
			| 	return_stat 
			| 	break_stat 
			| 	continue_stat 
			| 	do_stat

var_or_func -> NAME{suffix}

suffix -> (func_call) | [expr] | .STRING

assignment -> {, var_or_func} = explist -- 变量赋值或调用函数

explist -> expr{, expr}

expr -> binop_expr{logicop binop_expr} -- 表达式

logicop -> AND | OR -- 逻辑操作符

binop_expr -> unary_expr{binop unary_expr}

binop -> 
			== ~= > < >= <=  	-- 二元运算符
		| 	..               	
		| 	+ -				
		| 	* /

unary_expr -> {unary_op}simple_exp -- 一元表达式

unary_op -> -- 一元操作符
			NOT 
		| 	-
			
simple_exp -> -- 简单表达式
				NUMBER 
			| 	STRING 
			| 	NIL 
			|   var_or_func 
			|	'('expr')' 
			|	table_construct 
				  
table_construct -> '{'table_part [; table_part]'}'

table_part -> field_list | hash_fields

field_list -> expr {, expr}

hash_fields -> hash_field {, hash_field}

hash_field -> 
				STRING = expr 
			| 	[expr]= expr

func_call -> '('explist')'

explist -> explist1 -- 表达式列表（可为空）

if_stat -> cond THEN block {ELSEIF ifstat} [ELSE block] END

while_stat -> WHILE cond DO block END

for_stat -> FOR NAME = expr , expr [ , expr ] DO block END

cond -> expr

block -> chunk

local_stat -> LOCAL namelist [= explist1]

namelist -> STRING {, STRING}

func_stat -> FUNCTION [NAME] body

body -> (parlist) chunk END

parlist -> {STRING [,]}

return_stat -> RETURN explist

break_stat -> BREAK

continue_stat -> CONTINUE

do_stat -> DO block END