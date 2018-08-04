open Utils;
open Route;

type state = route;

type action =
  | ChangeView(route);

let component = ReasonReact.reducerComponent("Rtop");

let make = _children => {
  ...component,
  initialState: () =>
    ReasonReact.Router.dangerouslyGetInitialUrl() |. Route.urlToRoute,
  reducer: (action, _state) =>
    switch (action) {
    | ChangeView(view) => ReasonReact.Update(view)
    },
  didMount: self => {
    let watcherID =
      ReasonReact.Router.watchUrl(url =>
        Route.urlToRoute(url) |. ChangeView |. self.send
      );
    self.onUnmount(() => ReasonReact.Router.unwatchUrl(watcherID));
  },
  render: ({state}) =>
    <ReasonApollo.Provider client=GqlClient.instance>
      (
        switch (state) {
        | Home => <Home />
        | Note(noteInfo) => <Note noteInfo />
        | NoteNew => <Note_New />
        | AuthCallback(token) => <Auth.AuthCallback token />
        | AuthGithub => <Auth.AuthGithub />
        | AuthFailure => "auth failure" |. str
        | NotFound => <NotFound />
        }
      )
    </ReasonApollo.Provider>,
};

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
