# 


# November 9th, 2022

Having realized a potential test case for which I had not yet covered, 
I added a new test case to WPL. As this case passed, I did not update the submission file but, instead, left it on its own branch for incorporation into main at a later date. This was done, in part, to help keep main as similar to WPL as possible until the assignment was graded or I had a functioning individual project. 

# November 7th, 2022

Having only implemented the basic code for nested functions the prior day, I added a series of test cases for nested functions. Several of these confirmed suspicions I had that there were various issues with nested functions inluding their ability to access outside variables, and that that their use could result in name conflicts that, while passing the semantic visitor stage, cause code generation issues by attaching multiple entry blocks to a single function. 


# November 6th, 2022

To start off work, I decided to now go about working on implementing nested functions (not yet lambdas as these were not anonomous). As I had previously anticipated building the language around this possibility, my type system was already in a state where, theorietically, nested fuctions should be possible (arguments were stored on the type and not the symbol), and the grammar made nested functions legal. The problem, however, was that I was having issues generating nested functions (thus, I added explicit checks in WPL to disallow them). The issue with them was, after generating the code for a nested function, the IR Builder did not return to the insertion point of the outer function---thus leaving their definitions intertwined and invalid. However, using what I had learned from implementing forward declarations, I was able to fix this issue by saving and reverting the insertion point around function generation. 

At this point, I was able to define basic nested functions; however, my grammar did not provide a syntax to represent function/process types (thus, functions could only be stored as vars). To wrap up the day, I quickly implemented a type for functions in both the grammar and semantic visitor. 


# November 3rd, 2022

I started off work this day by doing some breif research on how lambdas and sum types may be represented in LLVM (I found both a website which describes some of the concepts and did some testing with emitting the IR of C and C++ files). After this, I wanted to try to implement basic lambdas (primarily as sum and product types seemed like they would require more extensive changes to the language). However, I found out that there was an unusual bug with the docker image for the project where, depending on host system, different versions of LLVM would be used. While I was unable to completley resolve this at the time, after a few hours, I found a workaround of manually uninstalling llvm-14 from the docker image and the adding the llvm-12 bin folder to the system path. 