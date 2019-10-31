slua = {
	ssd = {
		expr_stmt -> expr

		expr-> assign_expr

		assgin_expr -> 
						prefix_expr_list assign_op expr_list
					|	logical_expr

		prefix_expr_list -> prefix_expr {',' prefix_expr}

		prefix_expr -> 
						prefix_op {suffix_expr}
					|	suffix_expr {suffix_expr}

		prefix_op = '-'

		suffix_expr -> single_expr {suffix_op}

		single_expr -> 
						'(' ')'
					|	NIL
					|	FALSE
					|	TRUE
					|	immediate
					|	IDENTIFIER

		immediate -> 
						object_immediate
					|	function_difine
					|	STRING_IMMEDIATE
					|	NUMBER_IMMEDIATE

		object_immediate ->
						'{' '}'
					|	'{' object_immediate_item_list '}'

		object_immediate_item_list -> object_immediate_item {',' object_immediate_item}

		object_immediate_item -> 
						IDENTIFIER ':' object_immediate_item_value
					|	STRING_IMMEDIATE ':' object_immediate_item_value
					|	NUMBER_IMMEDIATE ':' object_immediate_item_value

		object_immediate_item_value -> immediate

		function_difine -> FUNCTION '(' [identifier_list] ')' stmt_sequence END

		identifier_list -> IDENTIFIER {',' IDENTIFIER}

		stmt_sequence ->
						stmt {stmt}
					|	ε

		suffix_op ->
						'(' ')'
					|	'(' expr_list ')'
					|	'[' expr ']'
					|	'.' IDENTIFIER

		expr_list -> expr {',' expr}

		assign_op -> 
						'='
					|	'+='
					|	'-='
					|	'*='
					|	'/='

		logical_expr -> 
				  		NOT compare_expr
					|	compare_expr logical_op logical_expr
					|	compare_expr

		compare_expr ->
				  		addtive_expr compare_op compare_expr
					| 	addtive_expr

		addtive_expr -> 
				  		multiply_expr addtive_op addtive_expr
					| 	multiply_expr

		multiply_expr ->
				  		prefix_expr multiply_op multiply_expr
					| 	prefix_expr

		logical_op ->
				  		AND
					| 	OR

		compare_op ->
				  		'<'
					| 	'>'
					| 	'<='
					| 	'>='
					| 	'=='
					| 	'!='

		addtive_op ->
				  		'+'
					| 	'-'

		multiply_op ->
				  		'*'
					| 	'/'
	}
}