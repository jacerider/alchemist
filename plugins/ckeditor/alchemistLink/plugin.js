/*
* Alchemist - text editor of awesomeness for Drupal
* by JaceRider and contributors. Available under the MIT license.
* See http://ashenrayne.com for more information
*/
(function() {
  var formSetup;

  this.Drupal.behaviors.alchemistLink = {
    attach: function(context, settings) {
      return jQuery('#alchemist-link-form', context).once(function() {
        return CKEDITOR.tools.callFunction(Drupal.alchemist.editor._.alchemistLinkFormSetup, jQuery(this), Drupal.alchemist.editor);
      });
    }
  };

  CKEDITOR.plugins.add("alchemistLink", {
    icons: "alchemistLink",
    created: false,
    init: function(editor) {
      var version;
      version = parseInt(CKEDITOR.version);
      editor.addCommand("alchemistLink", {
        allowedContent: 'a[!href]',
        requiredContent: 'a[href]',
        exec: function(editor) {
          var $element, base, element_settings;
          $element = jQuery(editor.element.$);
          if (!this.created) {
            this.created = true;
            base = $element.attr('id');
            element_settings = {
              url: "/alchemist/link/ajax",
              event: "onload",
              keypress: false,
              prevent: false,
              wrapper: base
            };
            Drupal.ajax[base] = new Drupal.ajax(base, $element, element_settings);
          }
          return $element.trigger('onload');
        }
      });
      editor.ui.addButton('alchemistLink', {
        label: 'Link',
        command: 'alchemistLink',
        toolbar: 'insert',
        icon: this.path + "/icons/alchemistLink.png"
      });
      if (version >= 4) {
        editor.setKeystroke(CKEDITOR.CTRL + 76, "alchemistLink");
      }
      editor.on("doubleclick", function(evt) {
        var element;
        delete evt.data.dialog;
        element = CKEDITOR.plugins.link.getSelectedLink(editor) || evt.data.element;
        if (!element.isReadOnly()) {
          if (element.is("a")) {
            editor.getSelection().selectElement(element);
            if (version >= 4) {
              return editor.commands.alchemistLink.exec();
            } else {
              if (version === 3) {
                return editor._.commands.alchemistLink.exec();
              }
            }
          }
        }
      });
      return editor._.alchemistLinkFormSetup = CKEDITOR.tools.addFunction(formSetup, editor);
    }
  });

  formSetup = function(form, editor) {
    var $selected, defaultUrl, selectedElement, selection, submit, title, web;
    submit = jQuery('.alchemist-link-save', form).removeClass('form-submit');
    title = jQuery('#alchemist-link-title', form);
    web = jQuery('#alchemist-link-web', form);
    newWindow = jQuery('#alchemist-link-new', form);
    defaultUrl = 'http://';
    selection = editor.getSelection();
    selectedElement = CKEDITOR.plugins.link.getSelectedLink(editor);
    if (selectedElement && selectedElement.hasAttribute('href')) {
      selection.selectElement(selectedElement);
    } else {
      selectedElement = null;
    }
    if (CKEDITOR.env.ie && typeof selection !== 'undefined') {
      selection.lock();
    }
    if (selectedElement) {
      $selected = jQuery(selectedElement.$);
      title.val($selected.text());
      if($selected.attr('target') == '_blank'){
        newWindow.prop('checked', true);
      }
      web.focus();
      web.val($selected.attr('href'));
      submit.val('Update Link');
    } else {
      title.val(CKEDITOR.tools.trim(selection.getSelectedText()));
      web.val(defaultUrl);
      title.focus();
      submit.val('Insert Link');
    }
    return submit.on('click', function(event) {
      var $wrapper, content, data, element, name, path, range, style, text;
      event.preventDefault();
      selection = editor.getSelection();
      path = web.val();
      title = title.val();
      data = {
        path: CKEDITOR.tools.trim(path),
        title: CKEDITOR.tools.trim(title),
        attributes: []
      };
      data.attributes["data-cke-saved-href"] = data.path;
      data.attributes.target = null;
      if (newWindow.is(":checked")){
        data.attributes.target = '_blank';
      }
      if (!selectedElement) {
        range = selection.getRanges(1)[0];
        if (range.collapsed) {
          content = (data.title ? data.title : data.path);
          text = new CKEDITOR.dom.text(content, editor.document);
          range.insertNode(text);
          range.selectNodeContents(text);
        }
        data.attributes.href = data.path;
        for (name in data.attributes) {
          if (data.attributes[name]) {
            null;
          } else {
            delete data.attributes[name];
          }
        }
        style = new CKEDITOR.style({
          element: "a",
          attributes: data.attributes
        });
        style.type = CKEDITOR.STYLE_INLINE;
        style.applyToRange(range);
        range.select();
      } else {
        element = selectedElement;
        element.setAttribute("href", data.path);
        element.setAttribute("data-cke-saved-href", data.path);
        element.setText((data.title ? data.title : data.path));
        for (name in data.attributes) {
          if (data.attributes[name]) {
            element.setAttribute(name, data.attributes[name]);
          } else {
            element.removeAttribute(name);
          }
        }
        selection.selectElement(element);
      }
      if (CKEDITOR.env.ie && typeof selection !== "undefined") {
        selection.unlock();
      }
      editor.fire('change');
      Drupal.alchemist.panelClose();
      return false;
    });
  };

}).call(this);
