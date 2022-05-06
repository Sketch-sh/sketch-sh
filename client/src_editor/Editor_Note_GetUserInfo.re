open Utils;
open GqlUserInfo;

let component = ReasonReact.statelessComponent("Editor_Note_GetUserInfo");

let make = (~userId, children) => {
  ...component,
  render: _self => {
    let query = UserInfoGql.make(~userId, ());
    <UserInfoComponent variables=query##variables>
      ...{({result}) =>
        switch (result) {
        | Loading => children(None)
        | Error(_) => children(None)
        | Data(response) =>
          response##user
          ->(
              arrayFirst(~empty=children(None), ~render=user =>
                children(Some(user))
              )
            )
        }
      }
    </UserInfoComponent>;
  },
};
