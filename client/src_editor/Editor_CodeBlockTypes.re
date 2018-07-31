module Widget = {
  type lineWidgetData =
    | Lw_Error(string)
    | Lw_Warning(string)
    | Lw_Value(string)
    | Lw_Stdout(string);
  type t = {
    lw_line: int,
    lw_data: lineWidgetData,
  };
};
