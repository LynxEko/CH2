
movzwl 0x4(%r10,%rsi,1),%edx
movzwl (%r10,%rsi,1),%r14d
movzwl 0x2(%r11,%rsi,1),%ecx
movzwl 0x2(%r9,%rsi,1),%r15d
cmp    %r14w,%dx
mov    %r14d,%eax
cmovbe %edx,%eax
movzwl %r15w,%edi
cmp    %r15w,%cx
cmovae %ecx,%edi
cmova  %r15d,%ecx
cmp    %ax,%cx
cmovb  %eax,%ecx
cmp    %r14w,%dx
cmovb  %r14d,%edx
movzwl %cx,%eax
cmp    %di,%dx
cmova  %edi,%edx
add    %edx,%eax
cltd
idiv   %r8d
mov    %dx,0x2(%rbx,%rsi,1)
add    $0x2,%rsi
cmp    %rsi,%rbp
