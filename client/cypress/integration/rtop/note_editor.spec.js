/// <reference types="Cypress" />

import {
  assertBlocks,
  assertCodeBlocks,
  assertTextBlocks,
  assertStdout,
  assertErrorsOrWarnings,
  assertValue,
  shortcut,
  assertLastBlockValue,
  typeTitle,
  typeBlock,
  aliasSaveButton,
} from "../../helpers/editor_helpers";

context("Note editor setup", () => {
  it("should not have smart indent", () => {
    cy.visit("new/reason");

    cy.get(".block__container")
      .first()
      .find("textarea")
      .as("block1")
      .type(
        `type foo('a) = {
  foo: 'a
};{Enter}a`,
        { force: true }
      );
    cy.window().then(win => {
      expect(win.editor.getValue()).to.equal(`type foo('a) = {
  foo: 'a
};
a`);
    });
  });
});

context("Copy value", () => {});

context("Foldable value", () => {
  it("should be foldable with large value", () => {
    cy.visit("new/reason");

    typeBlock(0, "include Pervasives;");
    shortcut("{ctrl}{enter}");

    assertValue(1);
    cy.get(".widget__value")
      .eq(0)
      .find("button")
      .should("be.visible");

    cy.get(".widget__foldable__gutter")
      .as("toggleFold")
      .should("exist")
      .click();
    assertValue(1);
    cy.get(".widget__value")
      .eq(0)
      .find("button")
      .should("not.be.visible");

    cy.get("@toggleFold").click({ force: true });
    cy.get(".widget__value")
      .eq(0)
      .find("button")
      .should("be.visible");
  });
  it("should not display gutter with small value", () => {
    cy.visit("new/reason");

    typeBlock(0, "let a = 1;");
    shortcut("{ctrl}{enter}");

    assertValue(1);

    cy.get(".widget__value")
      .eq(0)
      .find("button")
      .should("not.exist");

    cy.get(".widget__foldable__gutter").should("not.visible");
  });
});
