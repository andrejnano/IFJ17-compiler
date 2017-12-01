# Quick tests for ifj2017 compilator
# xmarko15 Peter Marko
# xmechl00 Stanislav Mechl
# xnanoa00 Andrej Nano
# xsvand06 Švanda Jan

from testc import *

def loadTests (tests):

    tests.add("SEM - Short", "scope\nend scope", [0])

    tests.add("LEX - Unknown char", "{", [1])
    tests.add("LEX - Unknown char", "%", [1])
    tests.add("LEX - Unknown char", "´", [1])
    tests.add("LEX - Unknown char", "ě", [1])
    tests.add("LEX - Float error", ".14", [1])
    tests.add("LEX - Float error", "13.", [1])
    tests.add("LEX - Float error", "1e", [1])
    tests.add("LEX - Float error", "1e+", [1])
    tests.add("LEX - Float error", "1e-", [1])
    tests.add("LEX - Float error", "1.e", [1])
    tests.add("LEX - Float error", "1.e-", [1])
    tests.add("LEX - Float error", "1.e+", [1])
    tests.add("LEX - Float error", "1..2", [1])
    tests.add("LEX - Single line comment", "scope'comment\nend scope", [0])
    tests.add("LEX - Single line comment", "scope'''' '' ' ¨¨%¨'\nend scope     '--!", [0])
    tests.add("LEX - Single line comment", "'scope scope scope\nscope 'scope\nend scope\n'end", [0])
    tests.add("LEX - Multiline comment", "/'comment'/scope/'c'/\nend/'space'/ scope", [0])
    tests.add("LEX - Multiline comment", "scope\nend/'''comment /'''/ scope", [0])
    tests.add("LEX - Multiline comment", "'/ scope\nscope\nend scope", [0])
    tests.add("LEX - Multiline comment", "/' end", [1])
    tests.add("LEX - Multiline comment", "scope\nend scope /' not closed '", [1])
    tests.add("LEX - Whitespace", "\t\n\t\tscope/'scope\nnext\n'/\n/'\n\n \t\nr'/end      \t  scope\n\n\n      \t\n", [0])
    tests.add("LEX - Whitespace empty", "scope /'scope\nnext'/\t     \n/'space between coments'/ end scope", [0])
    tests.add("LEX - String error", 'scope\ndim s as string = !"ssstring not closed by \\"\nend scope', [1])
    tests.add("LEX - String error", 'scope\ndim a as string = !""""\nend scope', [1])
    tests.add("LEX - String empty", 'scope\ndim b as string = !""\nend scope', [0])
    tests.add("LEX - String escape", 'scope\ndim c as string = !"\\002\\t\\n\\\\TEXtAbc\\065\\066\\067--\\"\\030\\"e"\nend scope', [0])
    tests.add("LEX - String ecs limit", 'scope\ndim limit as string = !"256\\256"\nend scope', [1])
    tests.add("LEX - String ecs limit", 'scope\ndim limit as string = !"265\\265"\nend scope', [1])
    tests.add("LEX - String ecs limit", 'scope\ndim limit as string = !"325\\325"\nend scope', [1])

    tests.add("SEM - Dim x", "scope\nDiM X as INTEger\nx    = 7  \n\nend scope", [0])

    tests.add("SYN - variable declarations", "scope\nDim x as Integer\nDim y as String\nDim z as Double\nend scope", [0])
    tests.add("SYN - variable definitions", "scope\nDim a as Integer = 42\nDim b as Double = 4.2\nDim c as Double = 3e-5\nend scope", [0])
    tests.add("SYN - function definition","function func() as Integer\n\nend function\n\nscope\nend scope", [0])
    tests.add("SYN - function definition with parameter","function func(n as Integer) as Integer\n\nend function\n\nscope\nend scope", [0])
    tests.add("SYN - function definition", "function func() as Integer\n\nend function\n\nscope\nend scope", [0])




    tests.add("SEM - Dim x", "scope\nDiM X as INTEger\n\nend scope", [0])
    tests.add("SEM - Dim x used", "scope\nDiM X as inTEger\nx    = 7  \n\nend scope", [0])
    tests.add("SEM - Dim x =", "scope\nDiM X as INTEger    = 3 \nend scope", [0])
    tests.add("SEM - Dim double", "scope\ndim var_name as double = 7\nend scope", [0])
    tests.add("SEM - Dim double add", "scope\ndim var_name as double = 2.7 + 3\nend scope", [0])
    tests.add("SEM - Dim double add int", "scope\ndim i as inteGER = 3\ndim var_name as double = 7 + i\nend scope", [0])

    tests.add("SEM - declaration after definition", """
    function func() as Double
        return 42
    end function
    
    Declare function func() as Double
    
    scope
    end scope
    """, [3])

    tests.add("SEM - undeclared variable assignment", """
    function func() as Integer
        dim x as Integer
        y = 22
    end function
    scope
    end scope
    """, [3])

    tests.add("SYN - function declaration inside scope", """
    scope
    declare function test() as Integer
    end scope
    """, [2])

    tests.add("SYN - function definition inside scope", """
    scope
    function test() as Integer
        return 0
    end function
    end scope
    """, [2])


    tests.add("PROG - Factorial", """'Výpočet faktoriálu (iterativně)
scope 'Hlavni telo programu
Dim a As Integer
DIM vysl AS INTEGER
PrinT !"Zadejte cislo pro vypocet faktorialu";
a = 9
If a < 0 THEN
print !"\\nFaktorial nelze spocitat\\n";
ELSE
Vysl = 1
Do WHile A > 0
VYSL = vysl * a
a = A - 1
LooP
Print !"\\nVysledek je:" ; vYsl ; !"\\n";
end IF
END SCOPE
    """, [0])

    tests.add("PROG - Factorial", """'Výpočet faktoriálu (rekurzivně)
Declare Function factorial (n As Integer) As Integer
Function factorial (n As Integer) As Integer
Dim temp_result As Integer
Dim decremented_n As Integer
Dim result As Integer
If n < 2 Then
result = 1
Else
decremented_n = n - 1
temp_result = factorial(decremented_n)
result = n * temp_result
End If
Return result
End Function
Scope 'Hlavni telo programu
Dim a As Integer
Dim vysl As Integer
Print !"Zadejte cislo pro vypocet faktorialu";
a = 9
If a < 0 Then
Print !"\\nFaktorial nelze spocitat\\n";
Else
vysl = factorial(a)
Print !"\\nVysledek je:" ; vysl ; !"\\n";
End If
End Scope
    """, [0])

    tests.add("PROG - Strings", """
Scope
Dim s1 As String
Dim s2 As String
Dim s1len As Integer
'Hlavni telo programu
s1 = !"Toto je nejaky text"
s2 = s1 + !", ktery jeste trochu obohatime"
Print s1; !"\\n"; s2; !"\\n";
s1len = Length(s1)
s1len = s1len - 4 + 1
s1 = SubStr(s2, s1len, 4)
Print !"4 znaky od "; s1len; !". znaku v \\""; s2; !"\\":"; s1; !"\\n";
Print !"Zadejte serazenou posloupnost vsech malych pismen a-h, ";
Print !"pricemz se pismena nesmeji v posloupnosti opakovat";
s1 = !"abcdefgh"
Do While (s1 <> !"abcdefgh")
Print !"\\nSpatne zadana posloupnost, zkuste znovu";
s1 = "abcdefgh"
Loop
End Scope
    """, [0])
