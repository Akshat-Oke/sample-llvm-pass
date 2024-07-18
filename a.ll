define i32 @foo() {
	ret i32 15
}

define void @bar() {
	ret void
}

define i32 @main() {
       %a = add i32 4, 5
       %h = add i8 4, 5
       %b = call i32 @foo()
       call void @bar()
       %c = add i32 %a, %b
       ret i32 %c
}  
