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