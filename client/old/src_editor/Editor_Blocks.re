/*
 * Tips: Fold code at level 2 would help alot with readability
 * If you're using VSCode, use <C-K> <C-2>
 */
[%%debugger.chrome];
Modules.require("./Editor_Blocks.css");

open Utils;
open Editor_Types;
open Editor_Types.Block;
open Editor_Blocks_Utils;
open Editor_Links_Utils;

type action =
  | Block_Add(id, blockTyp)
  | Block_Execute(bool, blockTyp)
  | Block_FocusNextBlockOrCreate(blockTyp)
  | Block_QueueDelete(id)
  | Block_DeleteQueued(id)
  | Block_Restore(id)
  | Block_Focus(id, blockTyp)
  | Block_Blur(id)
  | Block_UpdateValue(id, string, CodeMirror.EditorChange.t)
  | Block_AddWidgets(id, array(Widget.t))
  | Block_FocusUp(id)
  | Block_FocusDown(id)
  | Block_ChangeLanguage
  | Block_PrettyPrint
  | Block_CleanBlocksCopy
  | Block_MapRefmtToBlocks(list((id, string)));

module TimeoutMap = Belt.Map.String;

type state = {
  lang,
  blocks: array(block),
  blocksCopy: option(array(block)),
  deletedBlockMeta: ref(TimeoutMap.t(Js.Global.timeoutId)),
  stateUpdateReason: option(action),
  focusedBlock: option((id, blockTyp, focusChangeType)),
};

module Actions = {
  /*
   * Clean blocks copy
   * - This action is trigged when user made a changes to the content
   * - Because refmt between 2 languages is destructive
   *   We keep a copy of old state in `state.blocksCopy`
   *   so when you switch back to original language
   *   you'll get the original content
   */
  let cleanBlocksCopy = (action, state) =>
    ReasonReact.Update({
      ...state,
      blocksCopy: None,
      stateUpdateReason: Some(action),
    });

  let callRefmt = (operation, self) => {
    let id =
      Toplevel_Consumer.refmt(
        operation,
        self.ReasonReact.state.blocks->codeBlockDataPairs,
        fun
        | Belt.Result.Error(error) => Notify.error(error)
        | Belt.Result.Ok({hasError, result}) =>
          if (hasError) {
            /* TODO: Map syntax error to block position */
            Notify.error(
              "An error happened while formatting your code. It might be a syntax error",
            );
          } else {
            let result =
              result->Belt.List.reduce([], (acc, {refmt_id, refmt_value}) =>
                switch (refmt_value) {
                | Ok(code) => [(refmt_id, code), ...acc]
                | Error(_) => acc
                }
              );
            self.send(Block_MapRefmtToBlocks(result));
          },
      );
    self.onUnmount(() => Toplevel_Consumer.cancel(id));
  };
  let prettyPrint = (action, state) =>
    switch (state.lang) {
    | ML =>
      ReasonReact.UpdateWithSideEffects(
        {...state, stateUpdateReason: Some(action)},
        _self => Notify.info("Prettify ML code is not currently supported"),
      )
    | RE =>
      ReasonReact.UpdateWithSideEffects(
        {...state, stateUpdateReason: Some(action)},
        callRefmt(PrettyPrintRe),
      )
    };
  let changeLanguage = (action, state) =>
    switch (state.blocksCopy) {
    | None =>
      ReasonReact.UpdateWithSideEffects(
        {
          ...state,
          blocksCopy: Some(state.blocks),
          stateUpdateReason: Some(action),
        },
        callRefmt(
          switch (state.lang) {
          | ML => Toplevel.Types.ReToMl
          | RE => Toplevel.Types.MlToRe
          },
        ),
      )
    | Some(blocksCopy) =>
      ReasonReact.UpdateWithSideEffects(
        {
          ...state,
          blocksCopy: None,
          blocks: blocksCopy,
          stateUpdateReason: Some(action),
        },
        self => self.send(Block_Execute(false, BTyp_Code)),
      )
    };
  let mapRefmtToBlocks = (action, state, results) =>
    ReasonReact.UpdateWithSideEffects(
      {
        ...state,
        stateUpdateReason: Some(action),
        blocks:
          state.blocks
          ->(
              Belt.Array.mapU((. block) => {
                let {b_data, b_id, b_deleted} = block;
                b_deleted
                  ? block
                  : (
                    switch (b_data) {
                    | B_Code(bcode) =>
                      let mappedBlock =
                        results->Belt.List.getBy(((id, _code)) =>
                          id == b_id
                        );
                      switch (mappedBlock) {
                      | None => block
                      | Some((_id, code)) => {
                          ...block,
                          b_data:
                            B_Code({
                              ...bcode,
                              /* TODO: Don't remove widgets but executing and replace with latest widget */
                              bc_widgets: [||],
                              bc_value: code,
                            }),
                        }
                      };
                    | B_Text(_) => block
                    }
                  );
              })
            ),
      },
      ({send}) => send(Block_Execute(false, BTyp_Code)),
    );
  /*
   * Focus up helper
   * - Find current block index
   * - If it's the FIRST or not in blocks (should never happen) then ignore
   * - Else focus on upper block
   */
  let focusUp = (action, state, blockId) => {
    let blockIndex =
      state.blocks->arrayFindIndex(({b_id}) => blockId == b_id);
    switch (blockIndex) {
    | None
    | Some(0) => ReasonReact.NoUpdate
    | Some(index) =>
      let upperBlockIndex = index - 1;
      let {b_data, b_id} =
        state.blocks->Belt.Array.getUnsafe(upperBlockIndex);
      let blockTyp = blockDataToBlockTyp(b_data);
      ReasonReact.Update({
        ...state,
        stateUpdateReason: Some(action),
        focusedBlock: Some((b_id, blockTyp, FcTyp_BlockFocusUp)),
      });
    };
  };
  /*
   * Focus down helper
   * - Find current block index
   * - If it's the LAST or not in blocks (should never happen) then ignore
   * - Else focus on lower block
   */
  let focusDown = (action, state, blockId) => {
    let length = state.blocks->Belt.Array.length;
    let blockIndex =
      state.blocks->arrayFindIndex(({b_id}) => blockId == b_id);
    switch (blockIndex) {
    | None => ReasonReact.NoUpdate
    | Some(blockIndex) when blockIndex == length - 1 => ReasonReact.NoUpdate
    | Some(index) =>
      let lowerBlockIndex = index + 1;
      let {b_data, b_id} =
        state.blocks->Belt.Array.getUnsafe(lowerBlockIndex);
      let blockTyp = blockDataToBlockTyp(b_data);
      ReasonReact.Update({
        ...state,
        stateUpdateReason: Some(action),
        focusedBlock: Some((b_id, blockTyp, FcTyp_BlockFocusDown)),
      });
    };
  };
  /*
   * Add a new code/text block afterBlockId
   * It also focus on the newly added block
   */
  let add = (action, state, afterBlockId, blockTyp) => {
    let newBlockId = generateId();
    ReasonReact.Update({
      ...state,
      stateUpdateReason: Some(action),
      focusedBlock: Some((newBlockId, blockTyp, FcTyp_BlockNew)),
      blocks:
        state.blocks
        ->(
            Belt.Array.reduceU(
              [||],
              (. acc, block) => {
                let {b_id} = block;
                if (b_id != afterBlockId) {
                  Belt.Array.concat(acc, [|block|]);
                } else {
                  Belt.Array.concat(
                    acc,
                    [|
                      block,
                      {
                        b_id: newBlockId,
                        b_data:
                          switch (blockTyp) {
                          | BTyp_Text => emptyTextBlock()
                          | BTyp_Code => emptyCodeBlock()
                          },
                        b_deleted: false,
                      },
                    |],
                  );
                };
              },
            )
          )
        ->syncLineNumber,
    });
  };
  /*
   * Block Focus and Blur
   */
  /* TODO: Why blockTyp is needed? */
  let focus = (action, state, blockId, blockTyp) =>
    ReasonReact.Update({
      ...state,
      stateUpdateReason: Some(action),
      focusedBlock: Some((blockId, blockTyp, FcTyp_EditorFocus)),
    });
  let blur = (action, state, blockId) =>
    switch (state.focusedBlock) {
    | None => ReasonReact.NoUpdate
    | Some((focusedBlockId, _, _)) =>
      focusedBlockId == blockId
        ? ReasonReact.Update({
            ...state,
            stateUpdateReason: Some(action),
            focusedBlock: None,
          })
        : ReasonReact.NoUpdate
    };
  /*
   * Block delete and restore
   * - QueueDelete:
   *   + Pressed on BlockControls > DeleteButton
   *   + Set b_deleted to true and show fake block
   * - DeleteQueued: Timeout, remove blocks complete
   */
  let queueDelete = (action, state, blockId) => {
    let queueTimeout = self => {
      let timeoutId =
        Js.Global.setTimeout(
          () => self.ReasonReact.send(Block_DeleteQueued(blockId)),
          10000,
        );
      self.onUnmount(() => Js.Global.clearTimeout(timeoutId));
      state.deletedBlockMeta :=
        (state.deletedBlockMeta^)->TimeoutMap.set(blockId, timeoutId);
    };
    ReasonReact.UpdateWithSideEffects(
      {
        ...state,
        blocks:
          state.blocks
          ->(
              Belt.Array.mapU((. block) => {
                let {b_id} = block;
                b_id == blockId ? {...block, b_deleted: true} : block;
              })
            )
          ->syncLineNumber,
        stateUpdateReason: Some(action),
        focusedBlock:
          switch (state.focusedBlock) {
          | None => None
          | Some((focusedBlock, _, _)) =>
            focusedBlock == blockId ? None : state.focusedBlock
          },
      },
      queueTimeout,
    );
  };
  let deleteQueued = (action, state, blockId) => {
    /* Remove timeout in case user click on Delete immediately */
    switch ((state.deletedBlockMeta^)->TimeoutMap.get(blockId)) {
    | None => ()
    | Some(id) => Js.Global.clearTimeout(id)
    };
    state.deletedBlockMeta :=
      (state.deletedBlockMeta^)->TimeoutMap.remove(blockId);
    let blocksAfterDelete =
      state.blocks->(Belt.Array.keepU((. {b_id}) => b_id != blockId));
    if (Array.length(blocksAfterDelete) == 0) {
      let newBlockId = Utils.generateId();
      ReasonReact.UpdateWithSideEffects(
        {
          ...state,
          blocks: [|
            {b_id: newBlockId, b_data: emptyCodeBlock(), b_deleted: false},
          |],
          stateUpdateReason: Some(action),
          focusedBlock: Some((newBlockId, BTyp_Code, FcTyp_BlockNew)),
        },
        _self => {
          let deleteLastBlockAcknowledgedKey = "rtop:deleteLastBlockAcknowledged";
          Dom.Storage.(
            switch (localStorage |> getItem(deleteLastBlockAcknowledgedKey)) {
            | Some(_) => ()
            | None =>
              Notify.info(
                "When you delete all blocks, an empty code block will be created for you",
                ~sticky=true,
              );
              localStorage |> setItem(deleteLastBlockAcknowledgedKey, "1");
            }
          );
        },
      );
    } else {
      ReasonReact.Update({
        ...state,
        blocks: blocksAfterDelete->syncLineNumber,
        stateUpdateReason: Some(action),
        focusedBlock:
          switch (state.focusedBlock) {
          | None => None
          | Some((focusedBlock, _, _)) =>
            focusedBlock == blockId ? None : state.focusedBlock
          },
      });
    };
  };
  let restore = (action, state, blockId) => {
    let timeoutId = (state.deletedBlockMeta^)->TimeoutMap.get(blockId);
    switch (timeoutId) {
    | None => ReasonReact.NoUpdate
    | Some(timeoutId) =>
      state.deletedBlockMeta :=
        (state.deletedBlockMeta^)->TimeoutMap.remove(blockId);
      ReasonReact.UpdateWithSideEffects(
        {
          ...state,
          blocks:
            state.blocks
            ->(
                Belt.Array.mapU((. block) => {
                  let {b_id} = block;

                  b_id == blockId ? {...block, b_deleted: false} : block;
                })
              )
            ->syncLineNumber,
          stateUpdateReason: Some(action),
        },
        _self => Js.Global.clearTimeout(timeoutId),
      );
    };
  };
  /*
   * Execution and helpers
   * - Execute -> Map results to widgets
   * - Execute with shortcuts -> Focus on next block,
   *   if this is last then create new block
   */
  let execute =
      (action, state, focusNextBlock, blockTyp, onExecute, lang, links) => {
    let allCodeToExecute = codeBlockDataPairs(state.blocks);

    ReasonReact.UpdateWithSideEffects(
      {...state, stateUpdateReason: Some(action)},
      self => {
        if (focusNextBlock) {
          self.send(Block_FocusNextBlockOrCreate(blockTyp));
        };
        onExecute(true);
        open Belt.Result;

        let executeId =
          Toplevel_Consumer.execute(
            ~lang,
            ~blocks=allCodeToExecute,
            ~links=Array.to_list(links),
            fun
            | Error(error) => {
                onExecute(false);
                Notify.error(error);
              }
            | Ok((linksWithResults, blocksWithResults)) => {
                onExecute(false);

                blocksWithResults->(
                                     Belt.List.forEachU(
                                       (.
                                         {Toplevel.Types.id: blockId, result},
                                       ) => {
                                       let widgets =
                                         executeResultToWidget(result);
                                       self.send(
                                         Block_AddWidgets(blockId, widgets),
                                       );
                                     })
                                   );

                linksWithResults->Belt.List.forEachU((. linkResult) => {
                  let linkResult: Toplevel.Types.linkResult = linkResult;
                  let link: Editor_Types.Link.link = linkResult.link;
                  let result: Worker_Types.linkResult = linkResult.result;

                  switch (result) {
                  | Ok () => ()
                  | Error(message) =>
                    let name = getNameFromLink(link);
                    Notify.error(
                      {j|Module "$name" failed to link: $message|j},
                    );
                  };
                });
              },
          );

        self.onUnmount(() => Toplevel_Consumer.cancel(executeId));
      },
    );
  };
  let addWidgets = (action, state, blockId, widgets) =>
    ReasonReact.Update({
      ...state,
      stateUpdateReason: Some(action),
      blocks:
        state.blocks
        ->(
            Belt.Array.mapU((. block) => {
              let {b_id, b_data} = block;
              if (b_id != blockId) {
                block;
              } else {
                switch (b_data) {
                | B_Text(_) => block
                | B_Code(bcode) => {
                    ...block,
                    b_data: B_Code({...bcode, bc_widgets: widgets}),
                  }
                };
              };
            })
          ),
    });
  let focusNextBlockOrCreate = (action, state, blockTyp) => {
    let blockLength = state.blocks->Belt.Array.length;

    let currentBlockIndex =
      switch (state.focusedBlock) {
      | None => blockLength - 1
      | Some((id, _blockTyp, _)) =>
        switch (state.blocks->arrayFindIndex(({b_id}) => b_id == id)) {
        | None => blockLength - 1
        | Some(index) => index
        }
      };
    let findBlockId = index => {
      let {b_id, b_data} = state.blocks[index];
      (b_id, blockDataToBlockTyp(b_data));
    };
    if (currentBlockIndex == blockLength - 1) {
      ReasonReact.SideEffects(
        ({send}) =>
          send(Block_Add(findBlockId(currentBlockIndex)->fst, blockTyp)),
      );
    } else if (currentBlockIndex < blockLength - 1) {
      let (nextBlockId, nextBlockTyp) = findBlockId(currentBlockIndex + 1);
      ReasonReact.Update({
        ...state,
        stateUpdateReason: Some(action),
        focusedBlock:
          Some((
            nextBlockId,
            nextBlockTyp,
            FcTyp_BlockExecuteAndFocusNextBlock,
          )),
      });
    } else {
      ReasonReact.NoUpdate;
    };
  };
  /*
   * Update value
   * - Clean all widgets below the editing points to avoid stale state
   */
  let update = (action, state, blockId, newValue, diff) => {
    let blockIndex = state.blocks->getBlockIndex(blockId);
    ReasonReact.Update({
      ...state,
      stateUpdateReason: Some(action),
      blocks:
        state.blocks
        ->(
            Belt.Array.mapWithIndexU((. i, block) => {
              let {b_data} = block;
              if (i < blockIndex) {
                block;
              } else if (i == blockIndex) {
                switch (b_data) {
                | B_Code(bcode) => {
                    ...block,
                    b_data:
                      B_Code({
                        ...bcode,
                        bc_value: newValue,
                        bc_widgets: {
                          let removeWidgetBelowMe = diff->getFirstLineFromDiff;
                          let currentWidgets = bcode.bc_widgets;
                          currentWidgets->(
                                            Belt.Array.keepU(
                                              (. {Widget.lw_line, _}) =>
                                              lw_line < removeWidgetBelowMe
                                            )
                                          );
                        },
                      }),
                  }
                | B_Text(_) => {...block, b_data: B_Text(newValue)}
                };
              } else {
                switch (b_data) {
                | B_Text(_) => block
                | B_Code(bcode) => {
                    ...block,
                    b_data: B_Code({...bcode, bc_widgets: [||]}),
                  }
                };
              };
            })
          )
        ->syncLineNumber,
    });
  };
};

let blockControlsButtons = (blockId, isDeleted, send) =>
  <div className="block__controls--buttons">
    <UI_Balloon message="Add code block" position=Down>
      ...<button
           className="block__controls--button"
           ariaLabel="Add code block"
           onClick={_ => send(Block_Add(blockId, BTyp_Code))}>
           <Fi.Code />
           <sup> "+"->str </sup>
         </button>
    </UI_Balloon>
    <UI_Balloon message="Add text block" position=Down>
      ...<button
           className="block__controls--button"
           ariaLabel="Add text block"
           onClick={_ => send(Block_Add(blockId, BTyp_Text))}>
           <Fi.Edit2 />
           <sup> "+"->str </sup>
         </button>
    </UI_Balloon>
    {!isDeleted
       ? <UI_Balloon message="Delete block" position=Down>
           ...<button
                className="block__controls--button block__controls--danger"
                ariaLabel="Delete block"
                onClick={_ => send(Block_QueueDelete(blockId))}>
                <Fi.Trash2 />
                <sup> "-"->str </sup>
              </button>
         </UI_Balloon>
       : <UI_Balloon message="Restore block" position=Down>
           ...<button
                className="block__controls--button"
                ariaLabel="Restore block"
                onClick={_ => send(Block_Restore(blockId))}>
                <Fi.RefreshCw />
                <sup> "+"->str </sup>
              </button>
         </UI_Balloon>}
  </div>;

let component = ReasonReact.reducerComponent("Editor_Page");

[@react.component]
let make =
    (
      ~lang=RE,
      ~links: array(Link.link),
      ~blocks: array(block),
      ~readOnly=false,
      ~onUpdate,
      ~onExecute,
      ~registerExecuteCallback=?,
      ~registerShortcut: option(Shortcut.subscribeFun)=?,
      _children: ReactHelpers.childless,
    ) => {
  let makeInitialState = () => {
    lang,
    blocks: blocks->syncLineNumber,
    blocksCopy: None,
    deletedBlockMeta: ref(TimeoutMap.empty),
    stateUpdateReason: None,
    focusedBlock: None,
  };
  ReactCompat.useRecordApi({
    ...component,
    initialState: makeInitialState,
    willReceiveProps: ({state}) =>
      if (state.lang != lang) {
        {...state, lang};
      } else {
        state;
      },
    didMount: self => {
      self.send(Block_Execute(false, BTyp_Code));
      switch (registerExecuteCallback) {
      | None => ()
      | Some(register) =>
        register(() => self.send(Block_Execute(false, BTyp_Code)))
      };
      switch (registerShortcut) {
      | None => ()
      | Some(registerShortcut) =>
        let unReg =
          registerShortcut(
            ~global=true,
            "mod+enter",
            event => {
              open Webapi.Dom;

              event->KeyboardEvent.preventDefault;
              self.send(Block_Execute(false, BTyp_Code));
            },
          );
        let unReg2 =
          registerShortcut(
            ~global=true,
            "shift+enter",
            event => {
              open Webapi.Dom;

              event->KeyboardEvent.preventDefault;
              self.send(Block_Execute(true, BTyp_Code));
            },
          );
        let unReg3 =
          registerShortcut(
            ~global=true,
            "mod+shift+enter",
            event => {
              open Webapi.Dom;

              event->KeyboardEvent.preventDefault;
              self.send(Block_Execute(true, BTyp_Text));
            },
          );
        let unReg4 =
          registerShortcut(
            ~global=true,
            "mod+shift+i",
            event => {
              open Webapi.Dom;
              event->KeyboardEvent.preventDefault;
              self.send(Block_PrettyPrint);
            },
          );
        self.onUnmount(() => {
          unReg();
          unReg2();
          unReg3();
          unReg4();
        });
      };
    },
    didUpdate: ({oldSelf, newSelf}) => {
      if (oldSelf.state.lang != lang) {
        newSelf.send(Block_ChangeLanguage);
      };
      if (oldSelf.state.blocks !== newSelf.state.blocks) {
        let cleanBlocksCopyHelper = () =>
          switch (newSelf.state.blocksCopy) {
          | None => ()
          | Some(_) => newSelf.send(Block_CleanBlocksCopy)
          };
        switch (newSelf.state.stateUpdateReason) {
        | None => ()
        | Some(action) =>
          switch (action) {
          | Block_Focus(_, _)
          | Block_Blur(_)
          | Block_AddWidgets(_, _)
          | Block_FocusUp(_)
          | Block_FocusDown(_)
          | Block_ChangeLanguage
          | Block_CleanBlocksCopy
          | Block_FocusNextBlockOrCreate(_)
          | Block_Execute(_, _) => ()
          | Block_PrettyPrint
          | Block_MapRefmtToBlocks(_)
          | Block_Add(_, _)
          | Block_Restore(_)
          | Block_QueueDelete(_)
          | Block_DeleteQueued(_)
          | Block_UpdateValue(_, _, _) => onUpdate(newSelf.state.blocks)
          };

          switch (action) {
          | Block_Add(_, _)
          | Block_Restore(_)
          | Block_QueueDelete(_)
          | Block_DeleteQueued(_)
          | Block_PrettyPrint => cleanBlocksCopyHelper()
          | Block_UpdateValue(_, _, diff) =>
            switch (diff->CodeMirror.EditorChange.originGet) {
            | "setValue" => ()
            | _ => cleanBlocksCopyHelper()
            }
          | _ => ()
          };
        };
      };
    },
    reducer: (action, state) =>
      switch (action) {
      | Block_CleanBlocksCopy => Actions.cleanBlocksCopy(action, state)
      | Block_PrettyPrint => Actions.prettyPrint(action, state)
      | Block_ChangeLanguage => Actions.changeLanguage(action, state)
      | Block_MapRefmtToBlocks(results) =>
        Actions.mapRefmtToBlocks(action, state, results)
      | Block_AddWidgets(blockId, widgets) =>
        Actions.addWidgets(action, state, blockId, widgets)
      | Block_FocusNextBlockOrCreate(blockTyp) =>
        Actions.focusNextBlockOrCreate(action, state, blockTyp)
      | Block_Execute(focusNextBlock, blockTyp) =>
        Actions.execute(
          action,
          state,
          focusNextBlock,
          blockTyp,
          onExecute,
          lang,
          links,
        )
      | Block_UpdateValue(blockId, newValue, diff) =>
        Actions.update(action, state, blockId, newValue, diff)
      | Block_QueueDelete(blockId) =>
        Actions.queueDelete(action, state, blockId)
      | Block_DeleteQueued(blockId) =>
        Actions.deleteQueued(action, state, blockId)
      | Block_Restore(blockId) => Actions.restore(action, state, blockId)
      | Block_Focus(blockId, blockTyp) =>
        Actions.focus(action, state, blockId, blockTyp)
      | Block_Blur(blockId) => Actions.blur(action, state, blockId)
      | Block_Add(afterBlockId, blockTyp) =>
        Actions.add(action, state, afterBlockId, blockTyp)
      | Block_FocusUp(blockId) => Actions.focusUp(action, state, blockId)
      | Block_FocusDown(blockId) => Actions.focusDown(action, state, blockId)
      },
    render: ({send, state}) =>
      <>
        {state.blocks
         ->(
             Belt.Array.mapU((. {b_id, b_data, b_deleted}) =>
               b_deleted
                 ? <div key=b_id id=b_id className="block__container">
                     <div className="block__deleted">
                       <h3> "This block has been removed"->str </h3>
                       <p>
                         "It will be permanently deleted after 10 seconds"->str
                       </p>
                       <div className="block__deleted--buttons">
                         <button
                           className="block__deleted--button restore"
                           onClick={_ => send(Block_Restore(b_id))}
                           ariaLabel="Restore block">
                           <Fi.RefreshCw />
                           "Restore"->str
                         </button>
                         <button
                           className="block__deleted--button delete-immediately"
                           onClick={_ => send(Block_DeleteQueued(b_id))}
                           ariaLabel="Delete block immediately">
                           <Fi.Trash2 />
                           "Delete Immediately"->str
                         </button>
                       </div>
                       <div className="block__deleted--progress" />
                     </div>
                     <div className="block__controls">
                       {blockControlsButtons(b_id, b_deleted, send)}
                     </div>
                   </div>
                 : (
                   switch (b_data) {
                   | B_Code({bc_value, bc_widgets, bc_firstLineNumber}) =>
                     <div key=b_id id=b_id className="block__container">
                       <div className="source-editor">
                         <Editor_CodeBlock
                           value=bc_value
                           focused={
                             switch (state.focusedBlock) {
                             | None => None
                             | Some((id, _blockTyp, changeTyp)) =>
                               id == b_id ? Some(changeTyp) : None
                             }
                           }
                           onChange={(newValue, diff) =>
                             send(Block_UpdateValue(b_id, newValue, diff))
                           }
                           onBlur={() => send(Block_Blur(b_id))}
                           onFocus={() => send(Block_Focus(b_id, BTyp_Code))}
                           onBlockUp={() => send(Block_FocusUp(b_id))}
                           onBlockDown={() => send(Block_FocusDown(b_id))}
                           widgets=bc_widgets
                           readOnly
                           firstLineNumber=bc_firstLineNumber
                           lang
                         />
                       </div>
                       <div className="block__controls">
                         {readOnly
                            ? ReasonReact.null
                            : blockControlsButtons(b_id, b_deleted, send)}
                       </div>
                     </div>
                   | B_Text(text) =>
                     <div key=b_id id=b_id className="block__container">
                       <div className="text-editor">
                         <Editor_TextBlock
                           value=text
                           focused={
                             switch (state.focusedBlock) {
                             | None => None
                             | Some((id, _blockTyp, changeTyp)) =>
                               id == b_id ? Some(changeTyp) : None
                             }
                           }
                           onBlur={() => send(Block_Blur(b_id))}
                           onFocus={() => send(Block_Focus(b_id, BTyp_Text))}
                           onBlockUp={() => send(Block_FocusUp(b_id))}
                           onBlockDown={() => send(Block_FocusDown(b_id))}
                           onChange={(newValue, diff) =>
                             send(Block_UpdateValue(b_id, newValue, diff))
                           }
                           readOnly
                         />
                       </div>
                       {readOnly
                          ? ReasonReact.null
                          : <div className="block__controls">
                              {blockControlsButtons(b_id, b_deleted, send)}
                            </div>}
                     </div>
                   }
                 )
             )
           )
         ->ReasonReact.array}
      </>,
  });
};
