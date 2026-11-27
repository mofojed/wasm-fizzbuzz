(module $rust.wasm
  (type (;0;) (func (param i32)))
  (type (;1;) (func))
  (import "host" "putchar" (func $_RNvCsfYPiKK1XOFt_4rust7putchar (type 0)))
  (func $_RNvCs5WqGp7zLRnA_7___rustc17rust_begin_unwind (type 1)
    (local i32)
    i32.const -6
    local.set 0
    loop  ;; label = @1
      local.get 0
      i32.const 1048582
      i32.add
      i32.load8_u
      call $_RNvCsfYPiKK1XOFt_4rust7putchar
      local.get 0
      i32.const 1
      i32.add
      local.tee 0
      br_if 0 (;@1;)
    end
    i32.const 10
    call $_RNvCsfYPiKK1XOFt_4rust7putchar
    loop  ;; label = @1
      br 0 (;@1;)
    end)
  (func $main (type 1)
    (local i32 i32 i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    i32.const 16
    i32.sub
    local.tee 0
    global.set $__stack_pointer
    i32.const 0
    local.set 1
    block  ;; label = @1
      loop  ;; label = @2
        local.get 0
        i32.const 0
        i32.store offset=7 align=1
        local.get 0
        i64.const 0
        i64.store
        block  ;; label = @3
          block  ;; label = @4
            local.get 1
            i32.const 15
            i32.rem_u
            br_if 0 (;@4;)
            i32.const -8
            local.set 2
            loop  ;; label = @5
              local.get 2
              i32.const 1048590
              i32.add
              i32.load8_u
              call $_RNvCsfYPiKK1XOFt_4rust7putchar
              local.get 2
              i32.const 1
              i32.add
              local.tee 2
              br_if 0 (;@5;)
              br 2 (;@3;)
            end
          end
          block  ;; label = @4
            local.get 1
            i32.const 5
            i32.rem_u
            br_if 0 (;@4;)
            i32.const 4
            local.set 2
            loop  ;; label = @5
              local.get 2
              i32.const 1048582
              i32.add
              i32.load8_u
              call $_RNvCsfYPiKK1XOFt_4rust7putchar
              local.get 2
              i32.const 1
              i32.add
              local.tee 2
              i32.const 8
              i32.ne
              br_if 0 (;@5;)
              br 2 (;@3;)
            end
          end
          block  ;; label = @4
            block  ;; label = @5
              block  ;; label = @6
                local.get 1
                i32.const 3
                i32.rem_u
                i32.eqz
                br_if 0 (;@6;)
                i32.const 0
                local.set 3
                i32.const 1000000000
                local.set 2
                i32.const 1
                local.set 4
                local.get 1
                local.set 5
                loop  ;; label = @7
                  local.get 2
                  i32.eqz
                  br_if 3 (;@4;)
                  local.get 5
                  local.get 5
                  local.get 2
                  i32.div_s
                  local.tee 6
                  local.get 2
                  i32.mul
                  i32.sub
                  local.set 5
                  local.get 4
                  local.get 6
                  i32.eqz
                  i32.and
                  local.set 7
                  i32.const 1
                  local.set 4
                  local.get 2
                  i32.const 10
                  i32.div_u
                  local.tee 8
                  local.set 2
                  local.get 7
                  br_if 0 (;@7;)
                  local.get 3
                  i32.const 11
                  i32.eq
                  br_if 2 (;@5;)
                  local.get 0
                  local.get 3
                  i32.add
                  local.get 6
                  i32.const 48
                  i32.add
                  i32.store8
                  local.get 3
                  i32.const 1
                  i32.add
                  local.set 3
                  i32.const 0
                  local.set 4
                  local.get 8
                  local.set 2
                  br 0 (;@7;)
                end
              end
              i32.const -4
              local.set 2
              loop  ;; label = @6
                local.get 2
                i32.const 1048586
                i32.add
                i32.load8_u
                call $_RNvCsfYPiKK1XOFt_4rust7putchar
                local.get 2
                i32.const 1
                i32.add
                local.tee 2
                br_if 0 (;@6;)
                br 3 (;@3;)
              end
            end
            call $_RNvNtCsiErZIu7dPQ0_4core9panicking18panic_bounds_check
            unreachable
          end
          local.get 3
          i32.const 12
          i32.ge_u
          br_if 2 (;@1;)
          local.get 3
          i32.eqz
          br_if 0 (;@3;)
          i32.const 0
          local.set 2
          loop  ;; label = @4
            local.get 0
            local.get 2
            i32.add
            i32.load8_u
            call $_RNvCsfYPiKK1XOFt_4rust7putchar
            local.get 3
            local.get 2
            i32.const 1
            i32.add
            local.tee 2
            i32.ne
            br_if 0 (;@4;)
          end
        end
        i32.const 10
        call $_RNvCsfYPiKK1XOFt_4rust7putchar
        local.get 1
        i32.const 1
        i32.add
        local.tee 1
        i32.const 100
        i32.ne
        br_if 0 (;@2;)
      end
      local.get 0
      i32.const 16
      i32.add
      global.set $__stack_pointer
      return
    end
    local.get 3
    call $_RNvNtNtCsiErZIu7dPQ0_4core5slice5index16slice_index_fail
    unreachable)
  (func $_RNvNtCsiErZIu7dPQ0_4core9panicking18panic_bounds_check (type 1)
    call $_RNvNtCsiErZIu7dPQ0_4core9panicking9panic_fmt
    unreachable)
  (func $_RNvNtNtCsiErZIu7dPQ0_4core5slice5index16slice_index_fail (type 0) (param i32)
    call $_RNvNtCsiErZIu7dPQ0_4core9panicking9panic_fmt
    unreachable)
  (func $_RNvNtCsiErZIu7dPQ0_4core9panicking9panic_fmt (type 1)
    call $_RNvCs5WqGp7zLRnA_7___rustc17rust_begin_unwind
    unreachable)
  (memory (;0;) 17)
  (global $__stack_pointer (mut i32) (i32.const 1048576))
  (export "main" (func $main))
  (data $.rodata (i32.const 1048576) "PANIC!fizzbuzz"))
