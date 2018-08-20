/// <reference types="Cypress" />

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
