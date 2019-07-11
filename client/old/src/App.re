Modules.require("./App.css");
open Utils;
open Route;

type state = route;

type action =
  | ChangeView(route);

let component = ReasonReact.reducerComponent("Sketch.sh");

[@react.component]
let make = () =>
  ReactCompat.useRecordApi({
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
        <ButterToast.Tray />
        <Router.Unload.Provider />
        <AuthStatus.Provider />
        <FontFaceObserver.Provider />
        {switch (state) {
         | Home =>
           <Layout_WithTopbar>
             ...{
                  let%Epitath authState = children =>
                    <AuthStatus.IsAuthenticated>
                      ...children
                    </AuthStatus.IsAuthenticated>;

                  switch (authState) {
                  | Anonymous =>
                    let (lang, links, blocks) = Editor_Introduction.blocks;
                    <NoteNewLazy
                      title="Sketch.sh - Interactive ReasonML sketchbook"
                      blocks
                      links
                      lang
                    />;
                  | Login(userId) =>
                    <HomeLazy
                      fetch={() =>
                        [%bs.raw
                        {| import(/* webpackChunkName: "Home" */ "./Home.bs.js") |}]
                      }
                      onLoading={() => <UI_FullpageLoading />}
                      render={((module Home)) => <Home userId />}
                    />
                  };
                }
           </Layout_WithTopbar>
         | Note(noteInfo) =>
           <Layout_WithTopbar>
             ...<NoteLazy
                  fetch={() =>
                    %bs.raw
                    {| import(/* webpackChunkName: "Note" */ "./Note.bs.js") |}
                  }
                  onLoading={() => <UI_FullpageLoading />}
                  render={((module Note)) => <Note noteInfo />}
                />
           </Layout_WithTopbar>
         | NoteTemplateChoose =>
           <Layout_WithTopbar>
             ...<NoteTemplateChooseLazy
                  fetch={() =>
                    %bs.raw
                    {| import(/* webpackChunkName: "NoteTemplateChoose" */ "./NoteTemplateChoose.bs.js") |}
                  }
                  onLoading={() => <UI_FullpageLoading />}
                  render={((module NoteTemplateChoose)) =>
                    <NoteTemplateChoose />
                  }
                />
           </Layout_WithTopbar>
         | NoteNew(lang) =>
           <Layout_WithTopbar> ...<NoteNewLazy lang /> </Layout_WithTopbar>
         | User(userName) =>
           <Layout_WithTopbar>
             ...<UserLazy
                  fetch={() =>
                    %bs.raw
                    {| import(/* webpackChunkName: "User" */ "./User.bs.js") |}
                  }
                  onLoading={() => <UI_FullpageLoading />}
                  render={((module User)) => <User userName />}
                />
           </Layout_WithTopbar>
         | AuthCallback(token) => <Auth.AuthCallback token />
         | AuthLogout => <Logout />
         | AuthGithub => <Auth.AuthGithub />
         | AuthFailure => "auth failure"->str
         | NotFound => <NotFound />
         }}
        <UI_WarningStagingServer />
      </ReasonApollo.Provider>,
  });

let default = ReasonReact.wrapReasonForJs(~component, _jsProps => make([||]));
