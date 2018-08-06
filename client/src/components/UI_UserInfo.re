open GqlUserInfo;

open Utils;

let component = ReasonReact.statelessComponent("UI_UserInfo");
let make = (~userId, _children) => {
  ...component,
  render: _ => {
    let query = UserInfoGql.make(~userId, ());
    <UserInfoComponent variables=query##variables>
      ...(
           ({result}) =>
             switch (result) {
             | Loading => <div> (ReasonReact.string("Loading")) </div>
             | Error(error) =>
               <div> (ReasonReact.string(error##message)) </div>
             | Data(response) =>
               response##user_public
               |. arrayFirst(
                    ~empty="Can't find user with id" ++ userId |. str,
                    ~render=user =>
                    <dl>
                      (
                        user##name
                        =>> (
                          name =>
                            <Fragment>
                              <dt> ("Name" |. str) </dt>
                              <dd> (name |. str) </dd>
                            </Fragment>
                        )
                      )
                      (
                        user##username
                        =>> (
                          username =>
                            <Fragment>
                              <dt> ("Username" |. str) </dt>
                              <dd> (username |. str) </dd>
                            </Fragment>
                        )
                      )
                      (
                        user##avatar
                        =>> (
                          avatar =>
                            <img
                              src=avatar
                              width="80"
                              height="80"
                              alt=(
                                optionToEmptyString(user##username)
                                ++ " avatar"
                              )
                            />
                        )
                      )
                    </dl>
                  )
             }
         )
    </UserInfoComponent>;
  },
};
