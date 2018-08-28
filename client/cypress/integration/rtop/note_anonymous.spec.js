/// <reference types="Cypress" />
const faker = require("faker");

import {
  assertBlocks,
  assertCodeBlocks,
  assertTextBlocks,
  assertErrorsOrWarnings,
  assertValue,
  shortcut,
  assertLastBlockValue,
  typeTitle,
  typeBlock,
} from "../../helpers/editor_helpers";

import generate from "nanoid/generate";

const generateId = () =>
  generate(
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
    22
  );

context("create - edit", () => {
  it("create new and edit note anonymously", () => {
    cy.visit("new/reason");
    cy.get(".EditorNote__metadata--info").find(`a[href="/u/anonymous"]`);

    let title = faker.lorem.words();

    cy.get(".Topbar__action")
      .contains("Save")
      .as("save");

    cy.get("@save").should("be.disabled");

    cy.get(".EditorNote__metadata")
      .find("input")
      .first()
      .as("title")
      .type(title);

    cy.get("@save").should("be.enabled");

    cy.get("textarea")
      .first()
      .type("let a: string = 1;", { force: true });

    cy.get("@save").click();

    cy.url().should("match", /s\/.+/, "should not be new route");
    cy.get(".EditorNote__metadata--info")
      .find(`span`)
      .contains("last edited");

    cy.reload(true);

    cy.get("@title").should("have.value", title);

    cy.window().then(win => {
      expect(win.editor.getValue()).to.equal("let a: string = 1;");
    });

    /* Update note anonymously */
    cy.get("@save").should("be.disabled");

    cy.window().then(win => {
      win.editor.setValue("let a = 1;");
    });

    cy.url().then(firstUrl => {
      cy.get("@save")
        .should("be.enabled")
        .click();

      cy.window().then(win => {
        expect(win.editor.getValue()).to.equal("let a = 1;");
      });

      let noteId = firstUrl.split("/")[4];

      cy.url().should("contains", noteId, "same note id");
    });
  });
});

context("fork", () => {
  it("should not have fork button when creating new note", () => {
    cy.visit("new/reason");
    cy.get(`button[aria-label="Fork"]`).should("not.exist");
  });

  it("show confirmation when forking your own note", () => {
    cy.visit("new/reason");
    const stub = cy.stub();
    stub.onFirstCall().returns(false);
    cy.on("window:confirm", stub);

    let title = faker.lorem.words();
    cy.get(".EditorNote__metadata")
      .find("input")
      .first()
      .as("title")
      .type(title);

    shortcut("{ctrl}s");
    cy.url().should("match", /s\/.+/, "should not be new route");

    cy.url().then(currentUrl => {
      cy.get(`button[aria-label="Fork"]`).click();
      cy.url().should("equal", currentUrl);
    });
  });

  it("allow to fork your own note", () => {
    cy.visit("new/reason");
    const stub = cy.stub();
    stub.onFirstCall().returns(true);
    cy.on("window:confirm", stub);

    let title = faker.lorem.words();
    cy.get(".EditorNote__metadata")
      .find("input")
      .first()
      .as("title")
      .type(title);

    shortcut("{ctrl}s");
    cy.url().should("match", /s\/.+/, "should not be new route");

    cy.url().then(currentUrl => {
      cy.get(`button[aria-label="Fork"]`).click();
      cy.url().should("not.equal", currentUrl);
      cy.url().should("match", /s\/.+/, "should not be new route");
    });
  });

  it.only("allow navigate between forks", () => {
    cy.visit("new/reason");
    const stub = cy.stub();
    stub.withArgs("Changes you made may not be saved").returns(false);
    stub.withArgs("Do you want to fork your own Sketch?").returns(true);

    cy.on("window:confirm", stub);
    cy.get(".Topbar__action")
      .contains("Save")
      .as("save");

    typeTitle("1");
    typeBlock(0, "1");
    cy.get("@save").click();

    cy.url().should("match", /s\/.+/);

    let getId = url => {
      expect(url).to.match(/s\/.+/);
      return url.split("/")[4];
    };

    cy.url().then(firstUrl => {
      cy.get(`button[aria-label="Fork"]`).click();
      cy.url().should("not.eq", firstUrl);

      cy.url().then(secondUrl => {
        // Edit 2nd fork content
        typeTitle("{backspace}2");
        typeBlock(0, "2");

        cy.get("@save").should("be.enabled");
        cy.get("@save").click();
        cy.get("@save").should("be.disabled");

        cy.get(`button[aria-label="Fork"]`).click();
        cy.url().should("not.eq", secondUrl);

        // Edit 3rd fork content
        typeTitle("{backspace}3");
        typeBlock(0, "3");

        cy.get("@save").should("be.enabled");
        cy.get("@save").click();
        cy.get("@save").should("be.disabled");

        cy.get(`a[href="/s/${getId(secondUrl)}/"]`).click();
        cy.url().should("eq", secondUrl);
        assertBlocks(1);
        assertLastBlockValue("21");

        cy.get(`a[href="/s/${getId(firstUrl)}/"]`).click();
        cy.url().should("eq", firstUrl);
      });
    });
  });

  it("fork another anonymous", () => {
    let newNoteId = generateId();

    cy.request("POST", Cypress.env("GRAPHQL_ENDPOINT"), {
      operationName: "createNoteAnonymous",
      variables: {
        noteId: newNoteId,
        editToken: generateId(),
        userId: "anonymous",
        title: "",
        data: {
          lang: "RE",
          blocks: [
            {
              id: generateId(),
              data: { kind: "code", value: "let a = 1;" },
              deleted: false,
            },
          ],
        },
      },
      query: `mutation createNoteAnonymous(
          $noteId: String!, 
          $editToken: String!, 
          $userId: String!, 
          $title: String!, 
          $data: jsonb!
        ) { 
          mutate: insert_note(objects: 
            {title: $title, id: $noteId, user_id: $userId, data: $data}
          ) {
            returning {
              updated_at
            }
          }
          insert_note_edit_token(objects: 
            {note_id: $noteId, token: $editToken}
          ) {
            affected_rows  
           }
        }
          `,
    });
    cy.visit(`/s/` + newNoteId);
    cy.get(".block__container")
      .first()
      .find("textarea")
      .as("block1")
      .type("let a = 2;", { force: true });
    shortcut("{ctrl}s");
    cy.get(".Notify.Notify--error").contains("You don't have permission");
    cy.get(`button[aria-label="Fork"]`).click();

    cy.url().should("not.contain", newNoteId);
    cy.reload();
    assertBlocks(1);
    assertLastBlockValue("let a = 2;let a = 1;");
  });
});
