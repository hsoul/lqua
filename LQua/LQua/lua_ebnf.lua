lua = {
	chunk -> {stat[';']}[last_stat[';']]

	stat -> 
			varlist '=' exp_list
		| 	function_call
		| 	do_stat
		| 	while_stat
		| 	repeate_stat
		| 	if_stat
		| 	for_stat
		| 	FUNCTION func_name func_body 
		| 	local FUNCTION NAME func_body 
		| 	local name_list ['=' exp_list]

	expr_stat -> function_call | assignment

	primary_exp -> prefix_exp { '.' NAME | '[' exp ']'}

	assignment -> primary_exp {',' primary_exp} '=' exp_list

	varlist -> var {',' var}

	var -> NAME | primary_exp

	prefix_exp -> NAME | '(' exp ')'

	exp -> NIL | FALSE | TRUE | NUMBER | STRING | '...' | function | primary_exp | table_constructor | exp binop exp | unop exp

	exp_list -> {exp ','} exp

	last_stat -> return [exp_list] | BREAK

	function_call -> primary_exp [':' NAME func_args | func_args]

	do_stat -> DO block END

	block -> chunk

	while_stat -> WHILE exp DO block END

	repeate_stat -> REPEATE block UNITL exp

	if_stat -> IF exp THEN block {ELSEIF exp THEN block} [ELSE block] END

	func_stat -> FUNCTION func_name func_body

	func_name -> NAME{'.' NAME}[':' NAME]

	name_list -> NAME {',' NAME}

	func_args -> '(' [exp_list] ')' | table_constructor | STRING

	function -> FUNCTION func_body

	func_body -> '('[par_list]')' block END

	par_list -> name_list [',' '...'] | '...'

	for_stat -> FOR (for_num | for_list) END

	fornum -> NAME '=' exp1 ',' exp1 [',' exp1] for_body

	for_list -> NAME {',' NAME} IN exp_list for_body

	for_body -> DO block

	table_constructor -> '{' [field_list] '}'

	field_list -> field {field_sep field} [field_sep]

	field -> '[' exp ']' '=' exp | NAME '=' exp | exp

	field_sep -> ',' | ';'

	binop -> 
			'+' 
		| 	'-' 
		| 	'*' 
		| 	'/' 
		| 	'^' 
		| 	'%' 
		| 	'..' 
		| 	'<' 
		| 	'<=' 
		| 	'>' 
		| 	'>=' 
		| 	'==' 
		| 	'~=' 
		| 	AND 
		| 	OR

	unop -> 
			'-'
		|	NOT
		|	'#'
}