type error = [
  | `Default_role_is_not_in_allowed_roles(string, array(string), string)
];

/** Sign token for user */
let make:
  (~user_id: string, ~allowed_roles: array(string), ~default_role: string) =>
  result(string, [> error]);
