'use strict';

var Char = require("./char.js");
var List = require("./list.js");
var Block = require("./block.js");
var Bytes = require("./bytes.js");
var Stream = require("./stream.js");
var Hashtbl = require("./hashtbl.js");
var Caml_bytes = require("./caml_bytes.js");
var Caml_int32 = require("./caml_int32.js");
var Caml_format = require("./caml_format.js");
var Caml_builtin_exceptions = require("./caml_builtin_exceptions.js");

var initial_buffer = Caml_bytes.caml_create_bytes(32);

var buffer = /* record */[/* contents */initial_buffer];

var bufpos = /* record */[/* contents */0];

function reset_buffer(param) {
  buffer[0] = initial_buffer;
  bufpos[0] = 0;
  return /* () */0;
}

function store(c) {
  if (bufpos[0] >= buffer[0].length) {
    var newbuffer = Caml_bytes.caml_create_bytes((bufpos[0] << 1));
    Bytes.blit(buffer[0], 0, newbuffer, 0, bufpos[0]);
    buffer[0] = newbuffer;
  }
  buffer[0][bufpos[0]] = c;
  bufpos[0] = bufpos[0] + 1 | 0;
  return /* () */0;
}

function get_string(param) {
  var s = Bytes.sub_string(buffer[0], 0, bufpos[0]);
  buffer[0] = initial_buffer;
  return s;
}

function make_lexer(keywords) {
  var kwd_table = Hashtbl.create(undefined, 17);
  List.iter((function (s) {
          return Hashtbl.add(kwd_table, s, /* Kwd */Block.__(0, [s]));
        }), keywords);
  var ident_or_keyword = function (id) {
    try {
      return Hashtbl.find(kwd_table, id);
    }
    catch (exn){
      if (exn === Caml_builtin_exceptions.not_found) {
        return /* Ident */Block.__(1, [id]);
      } else {
        throw exn;
      }
    }
  };
  var keyword_or_error = function (c) {
    var s = Caml_bytes.bytes_to_string(Bytes.make(1, c));
    try {
      return Hashtbl.find(kwd_table, s);
    }
    catch (exn){
      if (exn === Caml_builtin_exceptions.not_found) {
        throw [
              Stream.$$Error,
              "Illegal character " + s
            ];
      }
      throw exn;
    }
  };
  var next_token = function (strm__) {
    while(true) {
      var match = Stream.peek(strm__);
      if (match !== undefined) {
        var c = match;
        var exit = 0;
        if (c < 124) {
          var switcher = c - 65 | 0;
          if (switcher > 57 || switcher < 0) {
            if (switcher >= 58) {
              exit = 1;
            } else {
              switch (switcher + 65 | 0) {
                case 9 : 
                case 10 : 
                case 12 : 
                case 13 : 
                case 26 : 
                case 32 : 
                    Stream.junk(strm__);
                    continue ;
                case 34 : 
                    Stream.junk(strm__);
                    reset_buffer(/* () */0);
                    return /* String */Block.__(4, [string(strm__)]);
                case 39 : 
                    Stream.junk(strm__);
                    var c$1;
                    try {
                      c$1 = $$char(strm__);
                    }
                    catch (exn){
                      if (exn === Stream.Failure) {
                        throw [
                              Stream.$$Error,
                              ""
                            ];
                      }
                      throw exn;
                    }
                    var match$1 = Stream.peek(strm__);
                    if (match$1 !== undefined) {
                      if (match$1 !== 39) {
                        throw [
                              Stream.$$Error,
                              ""
                            ];
                      }
                      Stream.junk(strm__);
                      return /* Char */Block.__(5, [c$1]);
                    } else {
                      throw [
                            Stream.$$Error,
                            ""
                          ];
                    }
                case 40 : 
                    Stream.junk(strm__);
                    var strm__$1 = strm__;
                    var match$2 = Stream.peek(strm__$1);
                    if (match$2 !== undefined && match$2 === 42) {
                      Stream.junk(strm__$1);
                      comment(strm__$1);
                      return next_token(strm__$1);
                    } else {
                      return keyword_or_error(/* "(" */40);
                    }
                case 45 : 
                    Stream.junk(strm__);
                    var strm__$2 = strm__;
                    var match$3 = Stream.peek(strm__$2);
                    if (match$3 !== undefined) {
                      var c$2 = match$3;
                      if (c$2 > 57 || c$2 < 48) {
                        reset_buffer(/* () */0);
                        store(/* "-" */45);
                        return ident2(strm__$2);
                      } else {
                        Stream.junk(strm__$2);
                        reset_buffer(/* () */0);
                        store(/* "-" */45);
                        store(c$2);
                        return number(strm__$2);
                      }
                    } else {
                      reset_buffer(/* () */0);
                      store(/* "-" */45);
                      return ident2(strm__$2);
                    }
                case 48 : 
                case 49 : 
                case 50 : 
                case 51 : 
                case 52 : 
                case 53 : 
                case 54 : 
                case 55 : 
                case 56 : 
                case 57 : 
                    exit = 4;
                    break;
                case 0 : 
                case 1 : 
                case 2 : 
                case 3 : 
                case 4 : 
                case 5 : 
                case 6 : 
                case 7 : 
                case 8 : 
                case 11 : 
                case 14 : 
                case 15 : 
                case 16 : 
                case 17 : 
                case 18 : 
                case 19 : 
                case 20 : 
                case 21 : 
                case 22 : 
                case 23 : 
                case 24 : 
                case 25 : 
                case 27 : 
                case 28 : 
                case 29 : 
                case 30 : 
                case 31 : 
                case 41 : 
                case 44 : 
                case 46 : 
                case 59 : 
                    exit = 1;
                    break;
                case 33 : 
                case 35 : 
                case 36 : 
                case 37 : 
                case 38 : 
                case 42 : 
                case 43 : 
                case 47 : 
                case 58 : 
                case 60 : 
                case 61 : 
                case 62 : 
                case 63 : 
                case 64 : 
                    exit = 3;
                    break;
                
              }
            }
          } else {
            switch (switcher) {
              case 27 : 
              case 29 : 
                  exit = 3;
                  break;
              case 30 : 
                  exit = 2;
                  break;
              case 26 : 
              case 28 : 
              case 31 : 
                  exit = 1;
                  break;
              default:
                exit = 2;
            }
          }
        } else {
          exit = c >= 127 ? (
              c >= 192 ? 2 : 1
            ) : (
              c !== 125 ? 3 : 1
            );
        }
        switch (exit) {
          case 1 : 
              Stream.junk(strm__);
              return keyword_or_error(c);
          case 2 : 
              Stream.junk(strm__);
              reset_buffer(/* () */0);
              store(c);
              var strm__$3 = strm__;
              while(true) {
                var match$4 = Stream.peek(strm__$3);
                if (match$4 !== undefined) {
                  var c$3 = match$4;
                  var exit$1 = 0;
                  if (c$3 >= 91) {
                    var switcher$1 = c$3 - 95 | 0;
                    if (switcher$1 > 27 || switcher$1 < 0) {
                      if (switcher$1 >= 97) {
                        exit$1 = 1;
                      } else {
                        return ident_or_keyword(get_string(/* () */0));
                      }
                    } else if (switcher$1 !== 1) {
                      exit$1 = 1;
                    } else {
                      return ident_or_keyword(get_string(/* () */0));
                    }
                  } else if (c$3 >= 48) {
                    if (c$3 > 64 || c$3 < 58) {
                      exit$1 = 1;
                    } else {
                      return ident_or_keyword(get_string(/* () */0));
                    }
                  } else if (c$3 !== 39) {
                    return ident_or_keyword(get_string(/* () */0));
                  } else {
                    exit$1 = 1;
                  }
                  if (exit$1 === 1) {
                    Stream.junk(strm__$3);
                    store(c$3);
                    continue ;
                  }
                  
                } else {
                  return ident_or_keyword(get_string(/* () */0));
                }
              };
          case 3 : 
              Stream.junk(strm__);
              reset_buffer(/* () */0);
              store(c);
              return ident2(strm__);
          case 4 : 
              Stream.junk(strm__);
              reset_buffer(/* () */0);
              store(c);
              return number(strm__);
          
        }
      } else {
        return undefined;
      }
    };
  };
  var ident2 = function (strm__) {
    while(true) {
      var match = Stream.peek(strm__);
      if (match !== undefined) {
        var c = match;
        var exit = 0;
        if (c >= 94) {
          var switcher = c - 95 | 0;
          if (switcher > 30 || switcher < 0) {
            if (switcher >= 32) {
              return ident_or_keyword(get_string(/* () */0));
            } else {
              exit = 1;
            }
          } else if (switcher !== 29) {
            return ident_or_keyword(get_string(/* () */0));
          } else {
            exit = 1;
          }
        } else if (c >= 65) {
          if (c !== 92) {
            return ident_or_keyword(get_string(/* () */0));
          } else {
            exit = 1;
          }
        } else if (c >= 33) {
          switch (c - 33 | 0) {
            case 1 : 
            case 6 : 
            case 7 : 
            case 8 : 
            case 11 : 
            case 13 : 
            case 15 : 
            case 16 : 
            case 17 : 
            case 18 : 
            case 19 : 
            case 20 : 
            case 21 : 
            case 22 : 
            case 23 : 
            case 24 : 
            case 26 : 
                return ident_or_keyword(get_string(/* () */0));
            case 0 : 
            case 2 : 
            case 3 : 
            case 4 : 
            case 5 : 
            case 9 : 
            case 10 : 
            case 12 : 
            case 14 : 
            case 25 : 
            case 27 : 
            case 28 : 
            case 29 : 
            case 30 : 
            case 31 : 
                exit = 1;
                break;
            
          }
        } else {
          return ident_or_keyword(get_string(/* () */0));
        }
        if (exit === 1) {
          Stream.junk(strm__);
          store(c);
          continue ;
        }
        
      } else {
        return ident_or_keyword(get_string(/* () */0));
      }
    };
  };
  var number = function (strm__) {
    while(true) {
      var match = Stream.peek(strm__);
      var exit = 0;
      if (match !== undefined) {
        var c = match;
        if (c >= 58) {
          if (c !== 69 && c !== 101) {
            exit = 1;
          } else {
            Stream.junk(strm__);
            store(/* "E" */69);
            return exponent_part(strm__);
          }
        } else if (c !== 46) {
          if (c >= 48) {
            Stream.junk(strm__);
            store(c);
            continue ;
          } else {
            exit = 1;
          }
        } else {
          Stream.junk(strm__);
          store(/* "." */46);
          var strm__$1 = strm__;
          while(true) {
            var match$1 = Stream.peek(strm__$1);
            var exit$1 = 0;
            if (match$1 !== undefined) {
              var c$1 = match$1;
              var switcher = c$1 - 69 | 0;
              if (switcher > 32 || switcher < 0) {
                if ((switcher + 21 >>> 0) > 9) {
                  exit$1 = 1;
                } else {
                  Stream.junk(strm__$1);
                  store(c$1);
                  continue ;
                }
              } else if (switcher > 31 || switcher < 1) {
                Stream.junk(strm__$1);
                store(/* "E" */69);
                return exponent_part(strm__$1);
              } else {
                exit$1 = 1;
              }
            } else {
              exit$1 = 1;
            }
            if (exit$1 === 1) {
              return /* Float */Block.__(3, [Caml_format.caml_float_of_string(get_string(/* () */0))]);
            }
            
          };
        }
      } else {
        exit = 1;
      }
      if (exit === 1) {
        return /* Int */Block.__(2, [Caml_format.caml_int_of_string(get_string(/* () */0))]);
      }
      
    };
  };
  var exponent_part = function (strm__) {
    var match = Stream.peek(strm__);
    if (match !== undefined) {
      var c = match;
      if (c !== 43 && c !== 45) {
        return end_exponent_part(strm__);
      } else {
        Stream.junk(strm__);
        store(c);
        return end_exponent_part(strm__);
      }
    } else {
      return end_exponent_part(strm__);
    }
  };
  var end_exponent_part = function (strm__) {
    while(true) {
      var match = Stream.peek(strm__);
      if (match !== undefined) {
        var c = match;
        if (c > 57 || c < 48) {
          return /* Float */Block.__(3, [Caml_format.caml_float_of_string(get_string(/* () */0))]);
        } else {
          Stream.junk(strm__);
          store(c);
          continue ;
        }
      } else {
        return /* Float */Block.__(3, [Caml_format.caml_float_of_string(get_string(/* () */0))]);
      }
    };
  };
  var string = function (strm__) {
    while(true) {
      var match = Stream.peek(strm__);
      if (match !== undefined) {
        var c = match;
        Stream.junk(strm__);
        if (c !== 34) {
          if (c !== 92) {
            store(c);
            continue ;
          } else {
            var c$1;
            try {
              c$1 = $$escape(strm__);
            }
            catch (exn){
              if (exn === Stream.Failure) {
                throw [
                      Stream.$$Error,
                      ""
                    ];
              }
              throw exn;
            }
            store(c$1);
            continue ;
          }
        } else {
          return get_string(/* () */0);
        }
      } else {
        throw Stream.Failure;
      }
    };
  };
  var $$char = function (strm__) {
    var match = Stream.peek(strm__);
    if (match !== undefined) {
      var c = match;
      Stream.junk(strm__);
      if (c !== 92) {
        return c;
      } else {
        try {
          return $$escape(strm__);
        }
        catch (exn){
          if (exn === Stream.Failure) {
            throw [
                  Stream.$$Error,
                  ""
                ];
          }
          throw exn;
        }
      }
    } else {
      throw Stream.Failure;
    }
  };
  var $$escape = function (strm__) {
    var match = Stream.peek(strm__);
    if (match !== undefined) {
      var c1 = match;
      if (c1 >= 58) {
        switch (c1) {
          case 110 : 
              Stream.junk(strm__);
              return /* "\n" */10;
          case 114 : 
              Stream.junk(strm__);
              return /* "\r" */13;
          case 111 : 
          case 112 : 
          case 113 : 
          case 115 : 
              Stream.junk(strm__);
              return c1;
          case 116 : 
              Stream.junk(strm__);
              return /* "\t" */9;
          default:
            Stream.junk(strm__);
            return c1;
        }
      } else {
        Stream.junk(strm__);
        if (c1 >= 48) {
          var match$1 = Stream.peek(strm__);
          if (match$1 !== undefined) {
            var c2 = match$1;
            if (c2 > 57 || c2 < 48) {
              throw [
                    Stream.$$Error,
                    ""
                  ];
            }
            Stream.junk(strm__);
            var match$2 = Stream.peek(strm__);
            if (match$2 !== undefined) {
              var c3 = match$2;
              if (c3 > 57 || c3 < 48) {
                throw [
                      Stream.$$Error,
                      ""
                    ];
              }
              Stream.junk(strm__);
              return Char.chr((Caml_int32.imul(c1 - 48 | 0, 100) + Caml_int32.imul(c2 - 48 | 0, 10) | 0) + (c3 - 48 | 0) | 0);
            } else {
              throw [
                    Stream.$$Error,
                    ""
                  ];
            }
          } else {
            throw [
                  Stream.$$Error,
                  ""
                ];
          }
        } else {
          return c1;
        }
      }
    } else {
      throw Stream.Failure;
    }
  };
  var comment = function (strm__) {
    while(true) {
      var match = Stream.peek(strm__);
      if (match !== undefined) {
        switch (match) {
          case 40 : 
              Stream.junk(strm__);
              var strm__$1 = strm__;
              var match$1 = Stream.peek(strm__$1);
              if (match$1 !== undefined) {
                if (match$1 !== 42) {
                  Stream.junk(strm__$1);
                  return comment(strm__$1);
                } else {
                  Stream.junk(strm__$1);
                  comment(strm__$1);
                  return comment(strm__$1);
                }
              } else {
                throw Stream.Failure;
              }
          case 41 : 
              Stream.junk(strm__);
              continue ;
          case 42 : 
              Stream.junk(strm__);
              var strm__$2 = strm__;
              while(true) {
                var match$2 = Stream.peek(strm__$2);
                if (match$2 !== undefined) {
                  var c = match$2;
                  Stream.junk(strm__$2);
                  if (c !== 41) {
                    if (c !== 42) {
                      return comment(strm__$2);
                    } else {
                      continue ;
                    }
                  } else {
                    return /* () */0;
                  }
                } else {
                  throw Stream.Failure;
                }
              };
          default:
            Stream.junk(strm__);
            continue ;
        }
      } else {
        throw Stream.Failure;
      }
    };
  };
  return (function (input) {
      return Stream.from((function (count) {
                    return next_token(input);
                  }));
    });
}

exports.make_lexer = make_lexer;
/* No side effect */
