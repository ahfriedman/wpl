# November 23rd, 2022

Having implemented the code to initialize structs, I then worked on making it so that way their fields could be accessed. To do this, I had to modify my initial fieldAccess rule (which previously was in the form of `VARIABLE '.' VARIABLE` and only allowed for array lengths) to `VARIABLE ('.' VARIABLE)+`. This was not too challenging to do as I could simply process this by loading the first `VARIABLE`, getting its type, making sure that the subsequent `VARIABLE` was the name of a field in this type, and then repeating the process. The only trick here was adding in a case for arrays so that way my language can still read their length. Similarly, in the code generation process, aside from an override to process array lengths, I am able to load the first `VARIABLE`, then lookup (and load) the fields by determining which element in the struct each subsequent field corresponds to. This, similarly to arrays, is done with a getelementpointer instruction. 

With this implementation in place, I started testing structs, and quickly found that field accesses did not work properly due to the fact that I was using a C++ map to lookup fields and which index a field corresponded to in the IR. This, however, was leading to inconsistent field accesses as the order I was accessing elements within a map was inconsistent. To solve this, I created my own wrapper around maps which functions similarly to a `LinkedHashMap` in java so that way I can track the order of elements. In doing so, I was able to resolve this problem. 



FIXME: DOES GEP WORK WITH VARIOUS TYPES?

# November 21th, 2022

Added basic implementation for struct initialization in the language. This attempt was largely based on how I implemented sum types due to the fact that, by nature of being a tagged union, their implementation requires the use of custom struct types in the LLVM IR. 


# November 19th, 2022

Added basic definition of a struct to the grammar file, created (but did not implement) visitor functions for them, and created a basic type to represent them. 

# November 18th, 2022

Today I fixed the bug I introduced last time where wpl would always attempt to generate an executable---even if there were previous compiler errors. In implementing this change, I also used an enum to create a compiler flag option for either no executable (default), to compile the executabel with clang, or to compile the executable with gcc. Finally, I switched from using a manual runtime object I had compiled to an automatically generated archive. 

# November 15th, 2022

With enums seeming mostly functional, I wanted to return to working on fixing the issue with lambdas where defining nested functions with the same name would result in conflicts. In order to do this, I wanted to start making whatever simplifications I could to the code generation around functions and processes. As such, I first combined the definition for FUNC and PROC into one definition. In addition, I wanted to implement automatic forward declarations as they would limit the number of cases that code generation would have to manage. Given that I already had implemented forward declarations with externs and that I already had the ability to generate nested functions, implementing this only required a quick additional pass in the semantic visitor to discover all functions prior to checking the implementations for each function. 

I then started to look at resolving the inital name conflict issue I had with nested functions; however, I found that, in order to make this work, I would have to manually rename the functions to resolve conflicts---otherwise I would not be able to look them up during function calls. Given this, and realizing that such a feature would obscure the use of higher-level functions, I decided to hold off on implementing this for now, and, instead, added a semantic error whenever such a case occurs. 

After this, I started working in improving the short circuiting implementation in my language so that way I would only need one phi node for each short. While writing the code for this was initially easy, the surprisingly challenging part was that, to account for this change, I modified the language grammar so that log ands were defined as `exprs+=expression (AND exprs+=expression)+` and, similarly, log ors were defined as `exprs+=expression (OR  exprs+=expression)+ `. The issue, however, was despite numerous attempts (changing the order of the grammar, writing new language rules, changing assoc, etc), I couldn't get the parser to give me an array of any more than two elements at a time. As such, I had to write a function to manually flatten parse trees when neeeded. 

To wrap up the day, as I had gotten tired of having to manually compile `.ll` files into executables, I followed the LLVM kaleidoscope for how to implement compiling to objects. This was midly tricky to do as, despite having the llvm dependencies for codegen, I was somehow getting undefined references to the llvm modules required for compiling objects. After much searching, I found an LLVM command for cmake to add more llvm modules to be linked during compiling which appeared to address the issue. 

In this preliminary implementation, all files get compiled to objects (simply as I wanted to test my ability to add in these features). In addition, I made it so that, after compiling the object files, my compiler invokes clang on them to produce an executable. 

# November 13th/14th, 2022

On both of these days, I added more test cases for enums. The only real change to the code ended up being due to identifying a bug with functions where I was not properly using their pointer type. 


# November 12th, 2022

Prior to today, I had been storing default values in the tag for the sum variable's type. As such, today I implemented a function which allows me to iterate through the ordered set used to store the possible types that a sum can hold. Given a specific type, this allows me to assign a unique tag for each type based on the types index in the set. 

This change, however, revealed an issue with my original implementation of WPL due to how type inference worked. This was because, as part of type inference, I was replacing the declared type of each variable with the more specific type it was assigned to. While this worked fine for WPL, it caused an issue now that I had sums and I needed to track which types were sum types. To address this, I simply made it so the declared type of a variable is kept except for when it is needed to be inferred. 

Having completed this, I then wanted to test my implementation. To do this, I started working on a match/type case statement. For this, I set up code generation so that way I would read the value of the sum's tag, and then use a switch to jump directly to the block of code corresponding to that case. This soon, however, revealed a problem that I could only create GEP expressions on allocations, but my rule for variables (as might appear as the expression that the code cases on) loads variables, which made them values. To solve this, I initially decided to add a second case to the variable rule which would just return the allocation if the variable was a sum type. While this worked, I thought this code was unlikely to scale, and so I instead made it so that way I re-allocate and store the sum types whenever needed so I can get pointers to them.

As this was all starting to work, I went back to my implementation of sum types as I was intially using a fixed size to hold their value. This was because I was unsure of how to get the size of arbitrary types. While I eventually found the `module->getDataLayout().getTypeAllocSize()` method, in the code I was using to generate types, I only had access to the module's context. Because of this, I had to change my code for getting llvm types to provide them with full access to the module. 

With this working, I went back to testing my sum types; however, I found that, while primitive types worked, more complex function types did not due to the fact that I was not treating them as singletons. To address this, having realized that, by definition, for the parser, each type would have to have a unique string representation, I made a custom comparator for my sum type's set which determines a type's equality based on the unique string representatuons I was already able to generate for debugging and error messages. The other issue that arose was that, in order to store a function in a sum, I needed to store a pointer to the function and not the function type itsself. While I had encountered this issue with storing functions as variables, it was previously easy to work around this by checking to see if the variable was a function and then using the `->getPointerTo();` function to get its type as a pointer. While I had done this as it let me keep my code for generating functions (which needed the non-pointer type) the same; however, as I started to need function types in more places, I decided that it would be more practical for my general function type to be a pointer type, and the I would simply take the `->getPointerElementType()` to get the actual function type for code generation. Having done this, I now had a basic sum type that, while probably inneficient, was working. In addition, due to my use of string identifiers, I was able to enable anonmous sum types (which made much more sense than forcing them to be pre-defined anyways).


# November 11th, 2022

Having gotten the basics of lambdas working, I wanted to move on to implementing sum types as I was worried that their implementation may require me to change how I manage types in code generation. This was because, I wasn't sure how adding the ability for arbitrarily named types would impact my ability to do semantic analysis. In addition, from my prior research, I knew that sum types would lay the foundation for me to implement structures/product types; however, they would not require me to rework the field access system as that would still be for arrays only. 

When working on this approach, I intially expected to use only pre-defined sum types despite the inherent limitations that these would have. My rational for this was that, anonomyous types would become too challenging to implement due to the fact that multiple types (and multiple permutations of the type) could be equivalent, and I would have to somehow reconcile those steps. Beause of this, I started my sum type system by adding a mechanism for representing a sum type in the grammar as well as a way to formally define them. I then moved on to implementing sum types. For this, I created a new type which was able to store a set of other types and then determine if a given type was within that set. 

Having done this, I then made some basic changes to my semantic visitor to account for the sum types. This ammounted to visiting definitions prior to visiting the rest of the code, generating the type of the definition, then binding that type to the symbol that represented the definition. Then, to account for the new case of custom defined variable names, I made that visitor attempt to lookup the symbol for that name, ensure that that symbol is a definition, and then return that type. 


I then moved on to code generation which was a much more challenging process. To start out, I knew that sum types would have to be treated as a product of two types: an integer (to represent the type of data stored in the sum) and a data type large enough to fit the largest type in the sum (which I would then cast back to the proper type based on the tag).

For this, I was able to modify my code for arrays to suit this puropose fairly easily; however, I spent a lot of time attempting to find ways of forcing LLVM to generate the type when presented with the type definition. In addition, I had an incredibly challenging time getting LLVM to allow me to re-use types. 

In my initial approach, for each sum type, I generated the llvm type it required and then had been re-using that. While this approach worked (and seemed to make the most sense as the types themselves did not know their own names), this would only work in llvm some times with many cases resulting in segmentation faults. After many hours attempting to debug the problem, I finally gave up and attempted to make the system work as if each sum had a name. When I did this, I was then able to find an obscure method which let me look up types by their name once they were defined. Using this somehow fixed my problems; however, I had hard coded a single name for every sum to use--something I would have to change for this to be versatile. 

# November 9th/10th, 2022

Having realized a potential test case for which I had not yet covered, 
I added a new test case to WPL. As this case passed, I did not update the submission file but, instead, left it on its own branch for incorporation into main at a later date. This was done, in part, to help keep main as similar to WPL as possible until the assignment was graded or I had a functioning individual project. 

Having added this test, I returned to my branch for implementing nested functions/lambdas. Having implemented basic nested functions, I then moved on to implementing basic lambdas. These were fairly simple to work on; however, they revealed a bug where I was unable to assign lambda variables to other lambda variables. To work around this, I eventually found that I needed to get pointers to the functions when treating them as variables (whereas my function type was treating them as llvm::Function s) as needed for their formal definition in code generation. 

Having completed this, I added a few more lambda tests to confirm my changes were working.

# November 8th, 2022

To fix the issue where functions were able to access variables in the local scope around them, I added two concepts to my language's system for managing symbols. The first was that of a definition: these would be things like functions which are immutable and can be visible within nested functions. The other was the concept of a stop: a way of setting the STManager to only lookup non-definitions below a certain depth (and definitions at all levels). This allows me to keep the STManager's use of a queue for scopes while allowing nested functions to call higher-level functions and keeping local variables at higher scopes invisible. 

# November 7th, 2022

Having only implemented the basic code for nested functions the prior day, I added a series of test cases for nested functions. Several of these confirmed suspicions I had that there were various issues with nested functions inluding their ability to access outside variables, and that that their use could result in name conflicts that, while passing the semantic visitor stage, cause code generation issues by attaching multiple entry blocks to a single function. 


# November 6th, 2022

To start off work, I decided to now go about working on implementing nested functions (not yet lambdas as these were not anonomous). As I had previously anticipated building the language around this possibility, my type system was already in a state where, theorietically, nested fuctions should be possible (arguments were stored on the type and not the symbol), and the grammar made nested functions legal. The problem, however, was that I was having issues generating nested functions (thus, I added explicit checks in WPL to disallow them). The issue with them was, after generating the code for a nested function, the IR Builder did not return to the insertion point of the outer function---thus leaving their definitions intertwined and invalid. However, using what I had learned from implementing forward declarations, I was able to fix this issue by saving and reverting the insertion point around function generation. 

At this point, I was able to define basic nested functions; however, my grammar did not provide a syntax to represent function/process types (thus, functions could only be stored as vars). To wrap up the day, I quickly implemented a type for functions in both the grammar and semantic visitor. 


# November 3rd, 2022

I started off work this day by doing some breif research on how lambdas and sum types may be represented in LLVM (I found both a website which describes some of the concepts and did some testing with emitting the IR of C and C++ files). After this, I wanted to try to implement basic lambdas (primarily as sum and product types seemed like they would require more extensive changes to the language). However, I found out that there was an unusual bug with the docker image for the project where, depending on host system, different versions of LLVM would be used. While I was unable to completley resolve this at the time, after a few hours, I found a workaround of manually uninstalling llvm-14 from the docker image and the adding the llvm-12 bin folder to the system path. 