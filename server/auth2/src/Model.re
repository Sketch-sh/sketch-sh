module D = Decode.AsResult.OfParseError;

module Github_profile = {
  type t = {
    github_id: string,
    username: string,
    name: option(string),
    email: option(string),
    avatar: option(string),
  };

  /*
    Passport attempts to normalize common information and put them in the return object
    Ray data from Github is stored in `_json` where `_raw` is stringify version.

    {
      displayName?: "",
      emails: [
        { value: "" }
      ],
      photos: [
        { value: ""}
      ],
      id,
      username,
      _json: {
        ..
        avatar_url: ""
      }
    }
   */

  let decoder = json => {
    let maybe_avatar = json => {
      D.field("_json", D.optionalField("avatar_url", D.string), json);
    };

    let make = (github_id, username, name, emails, photos) => {
      {
        github_id,
        username,
        name,
        email: emails->Option.flatMap(__x => Arr.get(__x, 0)),
        avatar: photos->Option.flatMap(__x => Arr.get(__x, 0)),
      };
    };
    open D.Pipeline;

    let values = array(D.field("value", D.string));
    let base_result =
      succeed(make)
      |> field("id", string)
      |> field("username", string)
      |> optionalField("displayName", string)
      |> optionalField("emails", values)
      |> optionalField("photos", values)
      |> run(json);

    base_result->Result.flatMapOk(gh_profile =>
      switch (gh_profile.avatar) {
      | Some(_) => Ok(gh_profile)
      | None =>
        maybe_avatar(json)
        ->Result.flatMapOk(avatar => Ok({...gh_profile, avatar}))
      }
    );
  };
};
