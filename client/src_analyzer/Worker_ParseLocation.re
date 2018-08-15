open Worker_Types.CompilerErrorMessage;

let parse = output =>
  (output |> Js.String.split({|File "", |}))
  ->(Belt.Array.keep(splitted => splitted->Js.String.trim != ""))
  ->(
      Belt.Array.reduceU(
        [||],
        (. acc, item) => {
          let matches =
            item
            |> Js.String.splitByRe(
                 [%bs.re {|/line\s(\d+), characters (\d+)-(\d+):\n/gi|}],
               );
          let item =
            switch (matches->Belt.Array.length) {
            | 5 =>
              let line = matches[1]->int_of_string;
              let content = matches[4]->Js.String.trim;
              let errContent = {
                o_content: content,
                o_loc: (
                  Worker_Types.CompilerErrorMessage.mkPos(
                    ~line,
                    ~col=matches[2]->int_of_string,
                  ),
                  Worker_Types.CompilerErrorMessage.mkPos(
                    ~line,
                    ~col=matches[3]->int_of_string,
                  ),
                ),
              };
              /* Error and Warning at the beginning */
              if (content |> Js.String.indexOf("Error") == 0) {
                Err_Error(errContent);
              } else if (content |> Js.String.indexOf("Warning") == 0) {
                Err_Warning(errContent);
              } else {
                Err_Unknown("File \"\", " ++ item);
              };
            | _ => Err_Unknown("File \"\", " ++ item)
            };
          Belt.Array.concat(acc, [|item|]);
        },
      )
    );
