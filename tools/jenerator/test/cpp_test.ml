open OUnit
open Cpp
open Syntax

 let test_parse_namespace _ =
   assert_equal ["fuga"; "hoge"] (parse_namespace "fuga::hoge")

 let test_gen_template _ =
   let names = Hashtbl.create 10 in
   assert_equal
     "t<bool, std::string>"
     (gen_template names "t" [Bool; String]);

   assert_equal
     "t<std::vector<bool> >"
     (gen_template names "t" [List Bool])

 let test_gen_type _ =
   let names = Hashtbl.create 10 in
   Hashtbl.add names "t" "name::t";
   assert_equal
     "name::t"
     (gen_type names (Struct "t"))

 let test_gen_string_literal _ =
   assert_equal
     "\"saitama\""
     (gen_string_literal "saitama");
   assert_equal
     "\"`~!@#$%^&*()-_=+[{]}\\\\|;:'\\\"\""
     (gen_string_literal "`~!@#$%^&*()-_=+[{]}\\|;:'\"")

 let test_gen_args _ =
   assert_equal
     "()"
     (gen_args []);
   assert_equal
     "(saitama)"
     (gen_args ["saitama"]);
   assert_equal
     "(saitama, gumma)"
     (gen_args ["saitama"; "gumma"])


 let suite = "cpp.ml" >:::
   [ "test_parse_namespace" >:: test_parse_namespace;
     "test_gen_template" >:: test_gen_template;
     "test_gen_type" >:: test_gen_type;
     "test_gen_string_literal" >:: test_gen_string_literal;
     "test_gen_args" >:: test_gen_args
   ]

 let _ =
   run_test_tt_main suite

