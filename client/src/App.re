Modules.require("./App.css");
open Utils;
open Route;

type state = route;

type action =
  | ChangeView(route);

let component = ReasonReact.reducerComponent("Sketch.sh");

let make = _children => {
  ...component,
  initialState: () =>
    ReasonReact.Router.dangerouslyGetInitialUrl()->Route.urlToRoute,
  reducer: (action, _state) =>
    switch (action) {
    | ChangeView(view) => ReasonReact.Update(view)
    },
  didMount: ({send, onUnmount}) => {
    let watcherID =
      ReasonReact.Router.watchUrl(url =>
        Route.urlToRoute(url)->ChangeView->send
      );
    onUnmount(() => ReasonReact.Router.unwatchUrl(watcherID));
  },
  render: ({state}) =>
    <ReasonApollo.Provider client=GqlClient.instance>
      <Helmet defaultTitle="Sketch.sh" titleTemplate="%s - Sketch.sh" />
      <Router.Unload.Provider />
      <UI_Notification.Provider />
      <AuthStatus.Provider />
      <FontFaceObserver.Provider />
      (
        switch (state) {
        | Home =>
          <Layout_WithTopbar>
            ...<AuthStatus.IsAuthenticated>
                 ...(
                      fun
                      | Anonymous =>
                        <NoteNewLazy
                          title="Sketch.sh - Interactive ReasonML sketchbook"
                          blocks=Editor_Introduction.blocks
                        />
                      | Login(userId) =>
                        <HomeLazy
                          fetch=(
                            () => [%bs.raw
                              {| import(/* webpackChunkName: "Home" */ "./Home.bs.js") |}
                            ]
                          )
                          onLoading=(() => <UI_FullpageLoading />)
                          render=(((module Home)) => <Home userId />)
                        />
                    )
               </AuthStatus.IsAuthenticated>
          </Layout_WithTopbar>
        | Note(noteInfo) =>
          <Layout_WithTopbar>
            ...<NoteLazy
                 fetch=(
                   () => [%bs.raw
                     {| import(/* webpackChunkName: "Note" */ "./Note.bs.js") |}
                   ]
                 )
                 onLoading=(() => <UI_FullpageLoading />)
                 render=(((module Note)) => <Note noteInfo />)
               />
          </Layout_WithTopbar>
        | NoteNew =>
          <Layout_WithTopbar> ...<NoteNewLazy /> </Layout_WithTopbar>
        | User(userName) =>
          <Layout_WithTopbar>
            ...<UserLazy
                 fetch=(
                   () => [%bs.raw
                     {| import(/* webpackChunkName: "User" */ "./User.bs.js") |}
                   ]
                 )
                 onLoading=(() => <UI_FullpageLoading />)
                 render=(((module User)) => <User userName />)
               />
          </Layout_WithTopbar>
        | AuthCallback(token) => <Auth.AuthCallback token />
        | AuthLogout => <Auth.AuthLogout />
        | AuthGithub => <Auth.AuthGithub />
        | AuthFailure => "auth failure"->str
        | NotFound => <NotFound />
        }
      )
      <UI_WarningStagingServer />
    </ReasonApollo.Provider>,
};

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
