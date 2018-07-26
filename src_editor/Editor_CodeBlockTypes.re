module Widget = {
  type t =
    | Lw_Error(widgetContent)
    | Lw_Value(widgetContent)
    | Lw_Stdout(widgetContent)
  and widgetContent = {
    content: string,
    line: int,
  };
};
