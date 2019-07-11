module Js = Js_of_ocaml.Js;

module Loc = {
  open Core.Loc;
  module Position = {
    open Position;
    let to_js = t => {
      %js
      {val line = t.line; val col = t.col};
    };
  };

  let to_js = t => {
    %js
    {
      {
        val locStart = t.locStart |> Position.to_js;
        val locEnd = t.locEnd |> Position.to_js
      };
    };
  };
};

module Evaluate = {
  open Util;
  open Core.Evaluate;
  let sub_to_js = sub =>
    sub
    |> Array.of_list
    |> Array.map(a => {
         let (loc, msg) = a;
         %js
         {
           val loc = loc |> Option.map(Loc.to_js) |> Js.Opt.option;
           val msg = msg |> Js.string
         };
       });
  let error_to_js = error => {
    %js
    {
      val errLoc = error.errLoc |> Option.map(Loc.to_js) |> Js.Opt.option;
      val errMsg = error.errMsg |> Js.string;
      val errSub = error.errSub |> sub_to_js
    };
  };

  let warning_to_js = warning => {
    %js
    {
      val warnLoc = warning.warnLoc |> Option.map(Loc.to_js) |> Js.Opt.option;
      val warnNumber = warning.warnNumber;
      val warnMsg = warning.warnMsg |> Js.string;
      val warnSub = warning.warnSub |> sub_to_js
    };
  };

  let blockContent_to_js =
    fun
    | BlockSuccess({msg, warnings}) => {
        %js
        {
          val typ = "BlockSuccess" |> Js.string;
          val payload =
            Js.Unsafe.inject(
              {
                %js
                {
                  val msg = msg |> Js.string;
                  val warnings =
                    warnings |> List.map(warning_to_js) |> Array.of_list
                };
              },
            )
        };
      }
    | BlockError({error, warnings}) => {
        %js
        {
          val typ = "BlockError" |> Js.string;
          val payload =
            Js.Unsafe.inject(
              {
                %js
                {
                  val msg = error |> error_to_js;
                  val warnings =
                    warnings |> List.map(warning_to_js) |> Array.of_list
                };
              },
            )
        };
      };

  let result_to_js: result => Js.t('a) =
    fun
    | Phrase({blockLoc, blockContent, blockStdout}) => {
        %js
        {
          val typ = "Phrase" |> Js.string;
          val payload =
            Js.Unsafe.inject(
              {
                %js
                {
                  val blockLoc =
                    blockLoc |> Option.map(Loc.to_js) |> Js.Opt.option;
                  val blockContent = blockContent |> blockContent_to_js;
                  val blockStdout = blockStdout |> Js.string
                };
              },
            )
        };
      }
    | Directive(content) => {
        %js
        {
          val typ = "Directive" |> Js.string;
          val payload = Js.Unsafe.inject(content |> Js.string)
        };
      };
};
