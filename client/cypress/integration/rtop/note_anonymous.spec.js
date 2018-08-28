/// <reference types="Cypress" />
const faker = require("faker");

import {
  assertBlocks,
  assertCodeBlocks,
  assertTextBlocks,
  assertErrorsOrWarnings,
  assertValue,
  shortcut,
} from "../../helpers/editor_helpers";

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
});
