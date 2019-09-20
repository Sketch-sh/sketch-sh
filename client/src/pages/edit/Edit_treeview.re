open Utils;
open Ds;

module S = {
  open Css;
  open Ds_unit;

  let new_file_section =
    [paddingLeft(Space.s4), margin2(~v=Space.s3, ~h=`zero)]->style;

  let list =
    [listStyleType(`none), paddingLeft(zero), margin2(~v=`zero, ~h=`zero)]
    ->style;
  let file =
    [
      width(`percent(100.)),
      padding4(
        ~top=Space.s1,
        ~right=Space.s1,
        ~bottom=Space.s1,
        ~left=Space.s4,
      ),
      color(Color.dark_3),
      cursor(`pointer),
      hover([backgroundColor(Color.light_4)]),
    ]
    ->style;

  let file_active =
    [
      backgroundColor(Color.brand),
      color(Color.brand_invert),
      hover([backgroundColor(Color.brand)]),
    ]
    ->style;
};

[@react.component]
let make = (~files, ~active_file, ~on_file_click) => {
  <>
    <section className=S.new_file_section>
      <Button color=Button.Secondary size=`sm> "New file"->str </Button>
    </section>
    <section>
      <ul className=S.list>
        {files
         ->Belt.Map.String.reduce(
             [||],
             (acc, filename, _file) => {
               let is_active = active_file == filename;

               let _ =
                 Js.Array.push(
                   <li
                     key=filename
                     className={
                       is_active ? Css.merge([S.file, S.file_active]) : S.file
                     }
                     onClick={_ => on_file_click(filename)}>
                     filename->str
                   </li>,
                   acc,
                 );
               acc;
             },
           )
         ->React.array}
      </ul>
    </section>
  </>;
};
