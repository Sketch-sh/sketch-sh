module Array = {
  include Belt.Array;
  type t('a) = array('a);

  let has: (t('a), 'a) => bool =
    (arr, needle) => Js.Array.indexOf(needle, arr) != (-1);
};

module ImmutableArray = {
  type t('a) = array('a);

  [@bs.send] external _push_concat: (t('a), t('a), 'a) => t('a) = "concat";
  let push = (arr, new_entry) => [||]->_push_concat(arr, new_entry);

  [@bs.send]
  external _unshift_concat: (t('a), 'a, t('a)) => t('a) = "concat";
  let unshift = (arr, new_entry) => [||]->_unshift_concat(new_entry, arr);
};

module Promise = {
  external unsafe_reject: string => 'a = "%identity";
};

type result('ok, 'error) =
  Belt.Result.t('ok, 'error) = | Ok('ok) | Error('error);

module Result = {
  include Belt.Result;

  let flatMapOk = (result, f) =>
    switch (result) {
    | Ok(a) => f(a)
    | Error(err) => Error(err)
    };
};

module List = {
  include Belt.List;
};

module Option = {
  include Belt.Option;

  let flatMapNone = (opt, f) =>
    switch (opt) {
    | None => f()
    | Some(a) => Some(a)
    };
  
};

module Json = {
  include Js.Json;
  external unsafeObjectToJson: Js.t({..}) => Js.Json.t = "%identity";
  external unsafeAnyToJson: 'a => Js.Json.t = "%identity";
};

module String = {
  include Js.String2;
  /** Turn empty string to option */
  let toOption = str =>
    if (str == "") {
      None;
    } else {
      Some(str);
    };
};
