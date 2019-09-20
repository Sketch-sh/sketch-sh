'use strict';

var Caml_hash_primitive = require("./caml_hash_primitive.js");
var Caml_builtin_exceptions = require("./caml_builtin_exceptions.js");

function push_back(q, v) {
  var cell = {
    content: v,
    next: undefined
  };
  var match = q.last;
  if (match !== undefined) {
    q.length = q.length + 1 | 0;
    match.next = cell;
    q.last = cell;
    return /* () */0;
  } else {
    q.length = 1;
    q.first = cell;
    q.last = cell;
    return /* () */0;
  }
}

function unsafe_pop(q) {
  var cell = q.first;
  var content = cell.content;
  var next_cell = cell.next;
  if (next_cell !== undefined) {
    q.length = q.length - 1 | 0;
    q.first = next_cell;
    return content;
  } else {
    q.length = 0;
    q.first = undefined;
    q.last = undefined;
    return content;
  }
}

function caml_hash(count, _limit, seed, obj) {
  var hash = seed;
  if (typeof obj === "number") {
    var u = obj | 0;
    hash = Caml_hash_primitive.caml_hash_mix_int(hash, (u + u | 0) + 1 | 0);
    return Caml_hash_primitive.caml_hash_final_mix(hash);
  } else if (typeof obj === "string") {
    hash = Caml_hash_primitive.caml_hash_mix_string(hash, obj);
    return Caml_hash_primitive.caml_hash_final_mix(hash);
  } else {
    var queue = {
      length: 0,
      first: undefined,
      last: undefined
    };
    var num = count;
    push_back(queue, obj);
    num = num - 1 | 0;
    while(queue.length !== 0 && num > 0) {
      var obj$1 = unsafe_pop(queue);
      if (typeof obj$1 === "number") {
        var u$1 = obj$1 | 0;
        hash = Caml_hash_primitive.caml_hash_mix_int(hash, (u$1 + u$1 | 0) + 1 | 0);
        num = num - 1 | 0;
      } else if (typeof obj$1 === "string") {
        hash = Caml_hash_primitive.caml_hash_mix_string(hash, obj$1);
        num = num - 1 | 0;
      } else if (typeof obj$1 !== "boolean") {
        if (typeof obj$1 !== "undefined") {
          if (typeof obj$1 === "symbol") {
            throw [
                  Caml_builtin_exceptions.assert_failure,
                  /* tuple */[
                    "caml_hash.ml",
                    128,
                    8
                  ]
                ];
          }
          if (typeof obj$1 !== "function") {
            var size = obj$1.length;
            if (size !== undefined) {
              var obj_tag = obj$1.tag | 0;
              var tag = (size << 10) | obj_tag;
              if (tag === 248) {
                hash = Caml_hash_primitive.caml_hash_mix_int(hash, obj$1[1]);
              } else {
                hash = Caml_hash_primitive.caml_hash_mix_int(hash, tag);
                var v = size - 1 | 0;
                var block = v < num ? v : num;
                for(var i = 0; i <= block; ++i){
                  push_back(queue, obj$1[i]);
                }
              }
            }
            
          }
          
        }
        
      }
      
    };
    return Caml_hash_primitive.caml_hash_final_mix(hash);
  }
}

exports.caml_hash = caml_hash;
/* No side effect */
