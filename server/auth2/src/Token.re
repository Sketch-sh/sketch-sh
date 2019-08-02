let jwt_options =
  JWT.Sign_options.make(~expiresIn="10 minutes", ~algorithm=`RS512, ());

type error = [
  | `Default_role_is_not_in_allowed_roles(string, array(string), string)
];

let make:
  (~user_id: string, ~allowed_roles: array(string), ~default_role: string) =>
  result(string, [> error]) =
  (~user_id, ~allowed_roles, ~default_role) => {
    let result =
      allowed_roles->Arr.has(default_role)
        ? Ok({
            "https://hasura.io/jwt/claims": {
              "x-hasura-allowed-roles": allowed_roles,
              "x-hasura-default-role": default_role,
              "x-hasura-user-id": user_id,
            },
          })
        : Error(
            `Default_role_is_not_in_allowed_roles((
              __LOC__,
              allowed_roles,
              default_role,
            )),
          );
    result->Result.flatMapOk(payload => {
      let payload = Json.unsafeObjectToJson(payload);

      Ok(
        JWT.sign(~secret=Config.jwt_secret, ~payload, ~options=jwt_options),
      );
    });
  };
